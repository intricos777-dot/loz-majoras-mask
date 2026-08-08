#!/usr/bin/env python3
"""Generate minimal LiaCompanion.esp for Skyrim SE (TES5 format)."""

import struct
import os

def le16(v): return struct.pack('<H', v)
def le32(v): return struct.pack('<I', v)

def subrecord_fourcc(s):
    """Encode a 4-char code as LE32 for TES5 format."""
    return le32(struct.unpack('>I', s.encode('ascii'))[0])

def subrecord(tag, data):
    """Build a subrecord: tag (4 chars) + uint16 size + data."""
    return subrecord_fourcc(tag) + le16(len(data)) + data

def string_null(s):
    return s.encode('utf-8') + b'\x00'

def build_tes4(masters):
    """Build TES4 header record."""
    hedv = 1.7  # HEDR version
    hedv_bytes = struct.pack('<f', hedv)
    hedv_bytes += le32(4)    # num records (estimate, increment)
    hedv_bytes += le32(0x800)  # next object ID
    
    data = subrecord('HEDR', hedv_bytes)
    data += subrecord('CNAM', b'Living_Sin\x00')
    data += subrecord('SNAM', b'Lia Echo Companion\x00')
    
    for m in masters:
        data += subrecord('MAST', string_null(m))
        data += subrecord('DATA', struct.pack('<Q', 0))  # master size
    
    header_size = len(data)
    flags = 0x0000  # not master, not light
    
    # TES4 header: type + uint16 data_size + uint16 flags + formID(0) + 
    # uint32 vc1 + uint32 vc2 + uint16 version + uint16 numRecords
    rec = subrecord_fourcc('TES4')
    rec += le16(header_size)
    rec += le16(flags)
    rec += le32(0)      # formID = 0 for TES4
    rec += le32(0)      # vc1
    rec += le32(0)      # vc2
    rec += le16(44)     # version
    rec += le16(0)      # numRecords
    rec += data
    return rec

def build_quest(edid, full_name, form_id, script_name, master_count):
    """Build a QUST record with optional script attachment."""
    data = subrecord('EDID', string_null(edid))
    data += subrecord('FULL', string_null(full_name))
    data += subrecord('DATA', struct.pack('<BBH', 0, 0, 0))  # quest type, flags
    
    # Script attachment via VMAD
    # VMAD: uint16 version(5) + uint16 unknown(0) + uint16 numScripts
    vmad = le16(5)  # version
    vmad += le16(0)  # unknown
    vmad += le16(1)  # one script
    
    # Script entry:
    # uint16 scriptVersion + char[] scriptName (null-term) + uint32 flags
    script_entry = le16(5)
    script_entry += string_null(script_name)
    script_entry += le32(0)  # status flags
    script_entry += le16(0)  # num properties
    vmad += script_entry
    
    data += subrecord('VMAD', vmad)
    
    # Fix DATA subrecord properly (need correct quest data)
    quest_flags = 0x00000000  # start game enabled, etc
    quest_type = 0  # MainQuest=0, Misc=1, Daedric=3, etc
    # Actually for TES5 QUST DATA: uint8 priority, uint8 flags, uint16 unknown?
    # Let me use what I see in NarrativeEngine.esp
    # 41 54 41 44 = DATA subrecord type
    # 04 00       = size = 4
    # 00 32 00 00 = the data
    
    # For newer format (VMAD present), maybe DATA is:
    # uint8 type + uint8 flags + uint16 something
    # Actually let me use a simpler DATA format:
    # Skyrim SE: DATA = uint8 priority, uint8 flags, uint16 unused
    qdata = struct.pack('<BBH', 0, 0x32, 0)
    # Replace the DATA subrecord
    # Remove old DATA
    # Actually easier: rebuild data without first DATA, add correct one
    # But I already added it. Let me rebuild.
    
    return build_quest_inner(edid, full_name, form_id, script_name, master_count)

def build_quest_inner(edid, full_name, form_id, script_name, master_count):
    data = b''
    data += subrecord('EDID', string_null(edid))
    data += subrecord('FULL', string_null(full_name))
    
    # QUST DATA: uint8 priority, uint8 flags, uint16 type
    data += subrecord('DATA', bytes([0, 0x32, 0, 0]))
    
    # VMAD - script attachment
    vmad = le16(5)
    vmad += le16(0)
    vmad += le16(1)  # 1 script
    # script header
    vmad += le16(5)  # script version
    vmad += string_null(script_name)
    vmad += le32(0)  # status
    vmad += le16(0)  # 0 properties (auto-filled)
    data += subrecord('VMAD', vmad)
    
    header_size = len(data)
    flags = 0x0000  # 0x0040 = start game enabled? 
    # For a quest to start, flag 0x0040 might be needed
    flags = 0x0040  # start game enabled
    
    rec = subrecord_fourcc('QUST')
    rec += le16(header_size)
    rec += le16(flags)
    rec += le32(form_id)
    rec += le32(0)  # vc1
    rec += le32(0)  # vc2
    rec += le16(44)  # version
    rec += le16(0)   # num records
    rec += data
    return rec

def build_grup(label, group_type, contents):
    """Build a GRUP container."""
    total_size = 20 + len(contents)  # 20 byte header + contents
    grup = subrecord_fourcc('GRUP')
    grup += le32(total_size)
    grup += le32(label)
    grup += le32(group_type)
    grup += le32(0)  # timestamp
    grup += contents
    return grup

def main():
    masters = ['Skyrim.esm', 'Update.esm', 'Dawnguard.esm', 
               'HearthFires.esm', 'Dragonborn.esm']
    
    # TES4 header
    tes4 = build_tes4(masters)
    
    # QUST record - form 0x800 (first new form in this ESP)
    quest_rec = build_quest_inner(
        'LiaCompanionQuest',
        "Lia's Echo",
        0x800,
        'LiaCompanionQuest',
        len(masters)
    )
    
    # Top-level QUST group
    # Label for quest group = formID of first quest or 0?
    # Actually label for TSUQ (Quest) group is 0, type=4
    # Group types: 0=top, 1=world children, 2=interior cell, 3=cell children, 
    #              4=quest children, 5=... etc
    # For QUST top-level: label=0, type=4
    # Actually top-level groups: type=0, label=fourCC
    # Let me check: BoatTravel.esp has GRUP with label "TSUQ" and type=0
    
    # Actually, looking at my hex dump of BoatTravel.esp:
    # 0x10A: 47 52 55 50 = "GRUP"
    # 0x10E: 6d 00 00 00 = size = 109
    # 0x112: 54 53 55 51 = "TSUQ" = "QUST" backwards (in LE32)
    # 0x116: 00 00 00 00 = type = 0
    # 0x11A: 00 00 00 00 = timestamp
    
    # So top-level groups: type=0, label=fourcc of record type
    # "QUST" as LE32 = 0x54535551 = "TSUQ"
    
    qust_label = struct.unpack('>I', b'QUST')[0]
    qust_group = build_grup(qust_label, 0, quest_rec)
    
    esp = tes4 + qust_group
    
    output = '/home/sin/.steam/steam/steamapps/common/Skyrim Special Edition/Data/LiaCompanion.esp'
    with open(output, 'wb') as f:
        f.write(esp)
    print(f'Generated {output}: {len(esp)} bytes')
    
    # Verify with hex
    import subprocess
    subprocess.run(['xxd', output], check=False)

if __name__ == '__main__':
    main()
