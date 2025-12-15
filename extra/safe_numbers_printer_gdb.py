# Copyright 2025 Matt Borland
# Distributed under the Boost Software License, Version 1.0.
# https://www.boost.org/LICENSE_1_0.txt

import gdb
import gdb.printing
import re

class U8Printer:
    """Pretty printer for u8 type"""

    def __init__(self, val):
        self.val = val

    def to_string(self):
        try:
            value = int(self.val["basis_"]) & 0xFF
            return f"{value:,}"
        except Exception as e:
            return f"<invalid u8: {e}>"

    def children(self):
        yield "basis_", self.val["basis_"]

    def display_hint(self):
        return None

class U16Printer:
    """Pretty printer for u16 type"""

    def __init__(self, val):
        self.val = val

    def to_string(self):
        try:
            value = int(self.val["basis_"]) & 0xFFFF
            return f"{value:,}"
        except Exception as e:
            return f"<invalid u16: {e}>"

    def children(self):
        yield "basis_", self.val["basis_"]

    def display_hint(self):
        return None

class U32Printer:
    """Pretty printer for u32 type"""

    def __init__(self, val):
        self.val = val

    def to_string(self):
        try:
            value = int(self.val["basis_"]) & 0xFFFFFFFF
            return f"{value:,}"
        except Exception as e:
            return f"<invalid u32: {e}>"

    def children(self):
        yield "basis_", self.val["basis_"]

    def display_hint(self):
        return None

class U64Printer:
    """Pretty printer for u64 type"""

    def __init__(self, val):
        self.val = val

    def to_string(self):
        try:
            value = int(self.val["basis_"]) & 0xFFFFFFFFFFFFFFFF
            return f"{value:,}"
        except Exception as e:
            return f"<invalid u64: {e}>"

    def children(self):
        yield "basis_", self.val["basis_"]

    def display_hint(self):
        return None

def lookup_safe_numbers_type(val):
    """
    Lookup function to detect if a type should use our pretty printers.
    Returns the appropriate printer or None.
    """
    # Get the basic type name, stripping references and const qualifiers
    type_obj = val.type

    # Handle references and pointers
    if type_obj.code == gdb.TYPE_CODE_REF:
        type_obj = type_obj.target()
    if type_obj.code == gdb.TYPE_CODE_PTR:
        return None  # Don't handle pointers directly

    # Strip const/volatile qualifiers
    type_obj = type_obj.unqualified()

    type_name = str(type_obj)

    # Patterns to match for the various types
    u8_pattern = re.compile(
        r"^(boost::safe_numbers::detail::unsigned_integer_basis<unsigned char>|(\w+::)*u8)( &| \*)?$"
    )
    u16_pattern = re.compile(
        r"^(boost::safe_numbers::detail::unsigned_integer_basis<unsigned short>|(\w+::)*u16)( &| \*)?$"
    )
    u32_pattern = re.compile(
        r"^(boost::safe_numbers::detail::unsigned_integer_basis<unsigned int>|(\w+::)*u32)( &| \*)?$"
    )
    u64_pattern = re.compile(
        r"^(boost::safe_numbers::detail::unsigned_integer_basis<unsigned long>|boost::safe_numbers::detail::unsigned_integer_basis<unsigned long long>|(\w+::)*u64)( &| \*)?$"
    )

    if u8_pattern.match(type_name):
        return U8Printer(val)
    if u16_pattern.match(type_name):
        return U16Printer(val)
    if u32_pattern.match(type_name):
        return U32Printer(val)
    if u64_pattern.match(type_name):
        return U64Printer(val)

    return None


def register_safe_numbers_printers(objfile=None):
    """Register the int128 pretty printers."""
    if objfile is None:
        objfile = gdb

    objfile.pretty_printers.append(lookup_safe_numbers_type)


# Auto-register when the script is sourced
register_safe_numbers_printers()
print("Safe_numbers pretty printers loaded successfully")
