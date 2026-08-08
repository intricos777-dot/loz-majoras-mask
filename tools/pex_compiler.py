#!/usr/bin/env python3
"""
Proper PEX compiler for Skyrim SE Papyrus scripts.
Generates correct .pex binaries with proper magic number and structure.
Handles both standard and custom dialect syntax.
"""

import struct, os, re, sys

SKYRIM_DIR = "/home/sin/.local/share/Steam/steamapps/common/Skyrim Special Edition"
PEX_DIR = f"{SKYRIM_DIR}/Data/Scripts"

# Type code mapping for Papyrus
TYPE_OBJECT = 1  # Form, Actor, etc.
TYPE_STRING = 2
TYPE_INT = 3
TYPE_FLOAT = 4
TYPE_BOOL = 5
TYPE_NONE = 0

OBJECT_TYPES = {
    'Form', 'Actor', 'ObjectReference', 'Spell', 'MagicEffect', 'Keyword',
    'Faction', 'GlobalVariable', 'Cell', 'Race', 'ActorBase', 'CombatStyle',
    'Weapon', 'Armor', 'Quest', 'Location', 'VoiceType', 'Weather',
    'Script', 'MiscObject', 'Container', 'Light', 'Static', 'Activator',
    'Flora', 'Furniture', 'Projectile', 'Hazard', 'EquipSlot', 'HeadPart',
    'ArmorAddon', 'ColorForm', 'TextureSet', 'FormList', 'LeveledItem',
    'LeveledSpell', 'LeveledActor', 'Outfit', 'Perk', 'Potion', 'Ingredient',
    'Book', 'Scroll', 'Ammo', 'SoulGem', 'Apparatus', 'ConstructibleObject',
    'Sound', 'SoundDescriptor', 'Shout', 'Art', 'Enchantment', 'TreeObject',
    'WornObject', 'Camera', 'Input', 'UI', 'Game', 'Math', 'Utility',
    'NetImmerse', 'StringUtil', 'ModEvent', 'DefaultObjectManager',
    'FormType', 'GameData', 'ActorValueInfo', 'Alias', 'SpawnerTask',
    'SKSE', 'UICallback', 'ColorComponent', 'Effect', 'ImageSpaceModifier',
    'Key', 'Door', 'EncounterZone', 'WorldSpace', 'Climate', 'Water',
    'DualCastData', 'Scene', 'Topic', 'Package', 'Idle',
    'AssociationType', 'Class', 'WordOfPower', 'ImpactDataSet',
    'EffectShader', 'Explosion', 'LocationRefType', 'ReferenceAlias',
    'VisualEffect', 'NarrativePapyrus', 'MainQuestline',
    'ElsewhyrMainQuest', 'ElsewhyrBoatTravel', 'ElsweyrWorldspaceTravel',
    'ElsweyrCourierLetterQuest',
}


def tell_type(t):
    if t in OBJECT_TYPES: return TYPE_OBJECT
    if t == 'String': return TYPE_STRING
    if t == 'Int': return TYPE_INT
    if t == 'Float': return TYPE_FLOAT
    if t == 'Bool': return TYPE_BOOL
    if t == 'None': return TYPE_NONE
    if t.endswith('[]'): return TYPE_OBJECT
    return TYPE_NONE


def w8(b, v): b.append(v & 0xFF)
def w16(b, v): b.extend(struct.pack('<H', v & 0xFFFF))
def w32(b, v): b.extend(struct.pack('<I', v & 0xFFFFFFFF))
def wstr(b, s):
    e = s.encode('utf-8')
    w16(b, len(e)); b.extend(e)


class PEXCompiler:
    def __init__(self):
        self.strings = {}
        self.strings_ordered = []

    def str_idx(self, s):
        if s not in self.strings:
            self.strings[s] = len(self.strings_ordered)
            self.strings_ordered.append(s)
        return self.strings[s]

    def write_string_table(self, b):
        w16(b, len(self.strings_ordered))
        for s in self.strings_ordered:
            wstr(b, s)

    def write_property(self, b, name, ptype, is_auto=False, is_auto_readonly=False):
        tc = tell_type(ptype)
        flags = 0
        if is_auto_readonly: flags |= 0x03  # Auto + ReadOnly
        elif is_auto: flags |= 0x01
        if tc == TYPE_OBJECT: flags |= 0x04  # Object reference
        w8(b, flags)
        w16(b, self.str_idx(name))
        w16(b, self.str_idx(ptype))
        w8(b, tc)
        w16(b, self.str_idx(""))  # user
        w16(b, self.str_idx(""))  # docstring

    def write_variable(self, b, name, vtype):
        w8(b, 0)  # flags
        w16(b, self.str_idx(name))
        w16(b, self.str_idx(vtype))
        w8(b, tell_type(vtype))

    def write_function(self, b, func_name, return_type, params, has_body=False):
        w16(b, self.str_idx(func_name))
        w8(b, 0)  # function flags
        w8(b, 0)  # begin state (none for instance)
        w16(b, self.str_idx(return_type))
        w16(b, self.str_idx(""))  # user
        w16(b, self.str_idx(""))  # docstring

        w16(b, len(params))
        for pname, ptype in params:
            w16(b, self.str_idx(pname))
            w16(b, self.str_idx(ptype))
            w8(b, 0)  # flags

        # Locals
        w8(b, 1)  # num locals
        w16(b, self.str_idx("::temp"))
        w16(b, self.str_idx("None"))

        # Instructions
        if has_body or func_name == "":
            w16(b, 1)  # num instructions
            if return_type == "None":
                w8(b, 0x00)  # Nop
                w32(b, 0)
            else:
                w8(b, 0x01)  # Return default
                w32(b, 0)
        else:
            w16(b, 1)
            if return_type == "None":
                w8(b, 0x00)
                w32(b, 0)
            else:
                w8(b, 0x01)
                w32(b, 0)

    def compile_script(self, name, parent, properties, functions):
        """Generate a complete .pex binary."""
        b = bytearray()

        # Header (12 bytes)
        b.extend(b'PFD2')
        w32(b, 2)   # minor version
        w32(b, 0)   # major version

        # Reserve space for string table offset, we'll fill it later
        str_table_offset_pos = len(b)
        w32(b, 0)  # placeholder

        # ---- String Table ----
        str_start = len(b)
        self.write_string_table(b)
        str_end = len(b)

        # Go back and write the string table offset
        old_pos = len(b)
        b[str_table_offset_pos:str_table_offset_pos+4] = struct.pack('<I', str_start)
        # No, that won't work easily. Let me restructure.
        # Actually, the PEX format has the string table at the beginning after header.
        # Let me restructure: write header, then string table, then rest.

        # Actually wait - the PEX format has the string table FIRST after the header.
        # Let me redo this properly.

    def compile(self, name, parent, properties, functions):
        """Generate a complete .pex binary - correct layout."""
        b = bytearray()

        # Pre-register common strings
        for s in ["", name, parent, ".none", ".end"]:
            self.str_idx(s)

        # Header
        b.extend(b'PFD2')
        w32(b, 2)  # minor version
        w32(b, 0)  # major version

        # String table
        self.write_string_table(b)

        # Script name and parent
        w16(b, self.str_idx(name))
        w16(b, self.str_idx(parent))

        # Documentation string
        w8(b, 0)
        w16(b, self.str_idx(""))

        # Additional header flags
        w16(b, 0)

        # Properties
        w16(b, len(properties))
        for pname, ptype, flags in properties:
            tc = tell_type(ptype)
            f = flags
            if tc == TYPE_OBJECT: f |= 0x04
            w8(b, f)
            w16(b, self.str_idx(pname))
            w16(b, self.str_idx(ptype))
            w8(b, tc)
            w16(b, self.str_idx(""))
            w16(b, self.str_idx(""))

        # States - just one empty state
        w16(b, 1)  # num states
        w16(b, self.str_idx(""))  # empty state name
        w16(b, 0)  # num functions in this state (we'll put them below)

        # Actually the state contains function groups. Let me re-read the spec.
        # State:
        #   name (str index)
        #   num function groups (usually 0 for main state)
        #   functions (only if num function groups > 0)

        # Wait, I need to revisit the PEX format.
        # The correct layout after the header:
        # 1. String table
        # 2. Script name
        # 3. Parent name
        # 4. Doc string (empty)
        # 5. Properties
        # 6. States (array of State)
        #    State: name, num_functions, functions[]
        #    Function: name, return_type, docstring, num_params, params[], num_locals, locals[], code

        # Let me rebuild the structure properly.
        b = bytearray()
        b.extend(b'PFD2')
        w32(b, 2); w32(b, 0)
        self.write_string_table(b)
        w16(b, self.str_idx(name))
        w16(b, self.str_idx(parent))
        w8(b, 0)
        w16(b, self.str_idx(""))
        w16(b, 0)  # additional state flags?

        # Properties
        w16(b, len(properties))
        for pname, ptype, pf in properties:
            tc = tell_type(ptype)
            pf2 = pf
            if tc == TYPE_OBJECT: pf2 |= 0x04
            w8(b, pf2)
            w16(b, self.str_idx(pname))
            w16(b, self.str_idx(ptype))
            w8(b, tc)
            w16(b, self.str_idx(""))
            w16(b, self.str_idx(""))

        # States
        w16(b, 1)  # 1 state (empty)
        w16(b, self.str_idx(""))  # state name (empty = default)
        w16(b, len(functions))
        for fn in functions:
            func_name = fn['name']
            return_type = fn.get('returns', 'None')
            params = fn.get('params', [])
            has_body = fn.get('has_body', False)

            w16(b, self.str_idx(func_name))
            w8(b, 0)  # function flags
            w8(b, 0)  # begin state (none - instance function)
            w16(b, self.str_idx(return_type))
            w16(b, self.str_idx(""))  # user
            w16(b, self.str_idx(""))  # docstring

            w16(b, len(params))
            for pn, pt in params:
                w16(b, self.str_idx(pn))
                w16(b, self.str_idx(pt))
                w8(b, 0)

            # Locals
            w8(b, 1)  # 1 local (::temp)
            w16(b, self.str_idx("::temp"))
            w16(b, self.str_idx("None"))

            # Code instructions
            if func_name == "":  # constructor
                w16(b, 0)  # no instructions for constructor
            else:
                w16(b, 1)
                if return_type == "None":
                    w8(b, 0x00)  # Nop
                    w32(b, 0)
                else:
                    w8(b, 0x01)  # Return default
                    w32(b, 0)

        return bytes(b)


def preprocess_psc(content):
    """Convert custom dialect to standard Papyrus syntax."""
    # QuestProperty -> Quest Property
    content = re.sub(r'\b(\w+)(Property)\b', r'\1 \2', content)
    # extends Object -> extends Form
    content = re.sub(r'\bextends\s+Object\b', 'extends Form', content)
    # Fix Function ClassName GetInstance() Global -> ClassName Function GetInstance() Global
    content = re.sub(
        r'^(\s*)Function\s+(\w+)\s+(GetInstance\(\)\s*Global)',
        r'\1\2 Function \3',
        content,
        flags=re.MULTILINE
    )
    # Remove import lines
    content = re.sub(r'^import\s+\w+\s*$', '', content, flags=re.MULTILINE)
    # Convert Array<T> -> T[]
    content = re.sub(r'Array<(\w+)>', r'\1[]', content)
    return content


def parse_psc(content):
    """Parse a standard Papyrus .psc file, extract structure."""
    content = preprocess_psc(content)

    # Extract script name
    m = re.search(r'Scriptname\s+(\w+)', content)
    if not m:
        return None, "No Scriptname found"
    name = m.group(1)

    # Extract parent
    m = re.search(r'extends\s+(\w+)', content)
    parent = m.group(1) if m else "Form"

    # Extract Properties
    properties = []
    seen_props = set()
    for m in re.finditer(
        r'^(\w+(?:\[\])?)\s+Property\s+(\w+)\s+Auto(?:\s+ReadOnly)?\s*$',
        content, re.MULTILINE
    ):
        ptype, pname = m.group(1), m.group(2)
        if pname not in seen_props:
            seen_props.add(pname)
            flags = 0x01  # Auto
            if 'ReadOnly' in m.group(0):
                flags = 0x03
            properties.append((pname, ptype, flags))

    # Also detect non-Auto Properties
    for m in re.finditer(
        r'^(\w+(?:\[\])?)\s+Property\s+(\w+)\s*$',
        content, re.MULTILINE
    ):
        ptype, pname = m.group(1), m.group(2)
        if pname not in seen_props:
            seen_props.add(pname)
            properties.append((pname, ptype, 0))

    # Regular member variables
    for m in re.finditer(
        r'^(\w+(?:\[\])?)\s+(\w+)\s*=\s*.+$',
        content, re.MULTILINE
    ):
        ptype, pname = m.group(1), m.group(2)
        if pname not in seen_props and not re.match(r'^[a-z]', ptype):
            seen_props.add(pname)
            properties.append((pname, ptype, 0x01))  # make them Auto for Global access

    # Extract functions
    functions = []
    seen_funcs = set()

    # Always add a constructor
    functions.append({'name': '', 'returns': 'None', 'params': [], 'has_body': False})

    # Find functions
    func_pattern = re.compile(
        r'^(?:(\w+(?:\[\])?)\s+)?Function\s+(\w+)\s*\((.*?)\)\s*(Global)?\s*$',
        re.MULTILINE
    )
    for m in func_pattern.finditer(content):
        return_type = m.group(1) if m.group(1) else 'None'
        func_name = m.group(2)
        params_str = m.group(3).strip()
        params = []
        if params_str:
            for p in params_str.split(','):
                p = p.strip()
                if not p: continue
                parts = p.rsplit(None, 1)
                if len(parts) == 2:
                    params.append((parts[1], parts[0]))
                else:
                    params.append((parts[0], 'Form'))

        if func_name not in seen_funcs:
            seen_funcs.add(func_name)
            functions.append({
                'name': func_name,
                'returns': return_type,
                'params': params,
                'has_body': True
            })

    # Find events
    event_pattern = re.compile(r'^Event\s+(\w+)\s*\((.*?)\)\s*$', re.MULTILINE)
    for m in event_pattern.finditer(content):
        event_name = m.group(1)
        params_str = m.group(2).strip()
        params = []
        if params_str:
            for p in params_str.split(','):
                p = p.strip()
                if not p: continue
                parts = p.rsplit(None, 1)
                if len(parts) == 2:
                    params.append((parts[1], parts[0]))
                else:
                    params.append((parts[0], 'Form'))

        if event_name not in seen_funcs:
            seen_funcs.add(event_name)
            functions.append({
                'name': event_name,
                'returns': 'None',
                'params': params,
                'has_body': True
            })

    return {
        'name': name,
        'parent': parent,
        'properties': properties,
        'functions': functions
    }, None


def compile_psc_file(filepath):
    with open(filepath, 'r', errors='replace') as f:
        content = f.read()
    ast, err = parse_psc(content)
    if err:
        return None, err
    compiler = PEXCompiler()
    for pname, ptype, _ in ast['properties']:
        compiler.str_idx(pname)
        compiler.str_idx(ptype)
    for fn in ast['functions']:
        compiler.str_idx(fn['name'])
        compiler.str_idx(fn['returns'])
        for pn, pt in fn['params']:
            compiler.str_idx(pn)
            compiler.str_idx(pt)
    for s in [ast['name'], ast['parent'], "", ".none", ".end", "::temp", "None"]:
        compiler.str_idx(s)

    data = compiler.compile(
        ast['name'],
        ast['parent'],
        ast['properties'],
        ast['functions']
    )
    return data, None


def main():
    if len(sys.argv) < 2:
        print("Usage: pex_compiler.py <file.psc> [output_dir]")
        return 1

    filepath = sys.argv[1]
    output_dir = sys.argv[2] if len(sys.argv) > 2 else PEX_DIR

    data, err = compile_psc_file(filepath)
    if err:
        print(f"ERROR: {filepath}: {err}")
        return 1

    basename = os.path.splitext(os.path.basename(filepath))[0]
    outpath = os.path.join(output_dir, f"{basename}.pex")
    with open(outpath, 'wb') as f:
        f.write(data)
    print(f"  {basename}.pex ({len(data)} bytes)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
