#!/usr/bin/env python3
"""Generate .pex stubs for ALL .psc files in Data/Scripts/Source/"""

import struct, os, re

SKYRIM_DIR = "/home/sin/.steam/steam/steamapps/common/Skyrim Special Edition"
PEX_DIR = f"{SKYRIM_DIR}/Data/Scripts"
SRC_DIR = f"{SKYRIM_DIR}/Data/Scripts/Source"

def w8(b, v): b.append(v & 0xFF)
def w16(b, v): b.extend(struct.pack('<H', v & 0xFFFF))
def w32(b, v): b.extend(struct.pack('<I', v & 0xFFFFFFFF))
def wstr(b, s):
    e = s.encode('utf-8')
    w16(b, len(e)); b.extend(e)

def build_pex(script_name, parent_name, props, funcs):
    strings = {"", script_name, parent_name, ".none", ".end"}
    for _, pn, _ in props: strings.add(pn)
    for p, _, _ in props: strings.add(p)
    for fn, _, _ in funcs:
        strings.add(fn)
        if fn: strings.add(fn)
    ordered = sorted(strings)
    si = {s: i for i, s in enumerate(ordered)}

    type_map = {
        'None': 0, 'Int': 3, 'Float': 4, 'Bool': 5, 'String': 2,
        'Form': 1, 'Actor': 1, 'ObjectReference': 1, 'Spell': 1,
        'MagicEffect': 1, 'Keyword': 1, 'Faction': 1,
        'GlobalVariable': 1, 'Cell': 1, 'Race': 1, 'ActorBase': 1,
        'CombatStyle': 1, 'Weapon': 1, 'Armor': 1, 'Quest': 1,
        'Location': 1, 'VoiceType': 1, 'Weather': 1, 'Script': 1,
        'MiscObject': 1, 'Container': 1, 'Light': 1, 'Static': 1,
        'Activator': 1, 'Flora': 1, 'Furniture': 1, 'Projectile': 1,
        'Hazard': 1, 'EquipSlot': 1, 'HeadPart': 1, 'ArmorAddon': 1,
        'ColorForm': 1, 'TextureSet': 1, 'FormList': 1,
        'LeveledItem': 1, 'LeveledSpell': 1, 'Outfit': 1, 'Perk': 1,
        'Potion': 1, 'Ingredient': 1, 'Book': 1, 'Scroll': 1,
        'Ammo': 1, 'SoulGem': 1, 'Apparatus': 1, 'ConstructibleObject': 1,
        'Sound': 1, 'SoundDescriptor': 1, 'Shout': 1, 'Art': 1,
        'Enchantment': 1, 'TreeObject': 1, 'WornObject': 1, 'Camera': 1,
        'Input': 1, 'UI': 1, 'Game': 1, 'Math': 1, 'Utility': 1,
        'NetImmerse': 1, 'StringUtil': 1, 'ModEvent': 1,
        'DefaultObjectManager': 1, 'FormType': 1, 'GameData': 1,
        'ActorValueInfo': 1, 'Alias': 1, 'SpawnerTask': 1, 'SKSE': 1,
        'UICallback': 1, 'ColorComponent': 1,
    }

    b = bytearray()
    b.extend(b'\xfa\x57\xc0\xde')
    w32(b, 2); w32(b, 0); w32(b, 0)
    w16(b, len(ordered))
    for s in ordered: wstr(b, s)
    ix = lambda s: si.get(s, 0)
    w16(b, ix(script_name)); w16(b, ix(parent_name)); w8(b, 0); w16(b, ix(""))
    w16(b, len(props))
    for pt, pn, is_auto in props:
        ptype = type_map.get(pt, 0)
        flags = 0
        if is_auto: flags |= 0x01
        if ptype == 1: flags |= 0x04
        w8(b, flags); w16(b, ix(pn)); w16(b, ix(pt)); w8(b, ptype)
        w16(b, ix("")); w16(b, ix(""))
    w16(b, 1); w16(b, ix("")); w16(b, 0)
    w16(b, len(funcs))
    for fn, rt, params in funcs:
        w16(b, ix(fn)); w8(b, 0); w8(b, 0)
        w16(b, ix(rt)); w16(b, ix("")); w16(b, ix(""))
        w16(b, len(params))
        for pn, pt in params:
            w16(b, ix(pn)); w16(b, ix(pt)); w8(b, 0)
        instrs = [(0x00, [])] if rt == "None" else [(0x01, [0])]
        w8(b, 1); w16(b, len(instrs))
        for op, args in instrs:
            w8(b, op)
            for a in args:
                w32(b, a) if isinstance(a, int) else w16(b, ix(a))
        w16(b, 1); w16(b, ix("::temp")); w16(b, ix("None"))
    return bytes(b)


def parse_psc(content):
    m = re.search(r'Scriptname\s+(\w+)', content)
    if not m: return None
    name = m.group(1)
    m2 = re.search(r'extends\s+(\w+)', content)
    parent = m2.group(1) if m2 else "Form"
    props = []
    seen_pnames = set()
    for m3 in re.finditer(r'^(\w+(?:\[\])?)\s+Property\s+(\w+)', content, re.MULTILINE):
        pt, pn = m3.group(1), m3.group(2)
        is_auto = 'Auto' in content[m3.end():m3.end()+10] if m3.end() < len(content) else False
        if pn not in seen_pnames:
            seen_pnames.add(pn)
            props.append((pt, pn, is_auto))
    funcs = [("", "None", [])]  # default constructor
    seen_fnames = set()
    for m4 in re.finditer(r'^\s*(?:Function|Event)\s+(\w+)', content, re.MULTILINE):
        fn = m4.group(1)
        if fn not in seen_fnames:
            seen_fnames.add(fn)
            funcs.append((fn, "None", []))
    return build_pex(name, parent, props, funcs)


def main():
    os.makedirs(PEX_DIR, exist_ok=True)
    files = sorted(f for f in os.listdir(SRC_DIR) if f.endswith('.psc'))
    gen = 0; err = 0
    for fname in files:
        pex_path = os.path.join(PEX_DIR, fname.replace('.psc', '.pex'))
        with open(os.path.join(SRC_DIR, fname), 'r', errors='replace') as f:
            content = f.read()
        data = parse_psc(content)
        if not data:
            err += 1
            continue
        with open(pex_path, 'wb') as f:
            f.write(data)
        gen += 1
        print(f"  {fname.replace('.psc', '.pex')} ({len(data)} bytes)")
    print(f"\nGenerated {gen}/{len(files)} .pex files ({err} failed)")

if __name__ == "__main__":
    main()
