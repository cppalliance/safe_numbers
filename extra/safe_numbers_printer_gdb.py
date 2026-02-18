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

class U128Printer:
    """Pretty printer for u128 type"""

    def __init__(self, val):
        self.val = val

    def to_string(self):
        try:
            basis = self.val["basis_"]
            low = int(basis["low"]) & 0xFFFFFFFFFFFFFFFF
            high = int(basis["high"]) & 0xFFFFFFFFFFFFFFFF
            value = (high << 64) | low
            return f"{value:,}"
        except Exception as e:
            return f"<invalid u128: {e}>"

    def children(self):
        yield "basis_", self.val["basis_"]

    def display_hint(self):
        return None

class BoundedUintPrinter:
    """Pretty printer for bounded_uint types"""

    def __init__(self, val):
        self.val = val

    def to_string(self):
        try:
            type_name = str(self.val.type)

            # Extract Min and Max from template parameters
            match = re.search(r'bounded_uint<([^,]+),\s*([^>]+)>', type_name)
            if match:
                min_str = match.group(1).strip()
                max_str = match.group(2).strip()
            else:
                min_str = "?"
                max_str = "?"

            # basis_ is a safe number type (u8/u16/u32/u64/u128),
            # which itself has a basis_ member with the raw value
            basis = self.val["basis_"]
            inner = basis["basis_"]

            # Check if this is a u128 (has low/high members)
            try:
                low = int(inner["low"]) & 0xFFFFFFFFFFFFFFFF
                high = int(inner["high"]) & 0xFFFFFFFFFFFFFFFF
                current = (high << 64) | low
            except gdb.error:
                byte_size = inner.type.sizeof
                mask = (1 << (byte_size * 8)) - 1
                current = int(inner) & mask

            return f"[{min_str}, {max_str}] {current:,}"
        except Exception as e:
            return f"<invalid bounded_uint: {e}>"

    def children(self):
        yield "basis_", self.val["basis_"]

    def display_hint(self):
        return None

class VerifiedU8Printer:
    """Pretty printer for verified_u8 type"""

    def __init__(self, val):
        self.val = val

    def to_string(self):
        try:
            value = int(self.val["basis_"]["basis_"]) & 0xFF
            return f"{value:,}"
        except Exception as e:
            return f"<invalid verified_u8: {e}>"

    def children(self):
        yield "basis_", self.val["basis_"]

    def display_hint(self):
        return None

class VerifiedU16Printer:
    """Pretty printer for verified_u16 type"""

    def __init__(self, val):
        self.val = val

    def to_string(self):
        try:
            value = int(self.val["basis_"]["basis_"]) & 0xFFFF
            return f"{value:,}"
        except Exception as e:
            return f"<invalid verified_u16: {e}>"

    def children(self):
        yield "basis_", self.val["basis_"]

    def display_hint(self):
        return None

class VerifiedU32Printer:
    """Pretty printer for verified_u32 type"""

    def __init__(self, val):
        self.val = val

    def to_string(self):
        try:
            value = int(self.val["basis_"]["basis_"]) & 0xFFFFFFFF
            return f"{value:,}"
        except Exception as e:
            return f"<invalid verified_u32: {e}>"

    def children(self):
        yield "basis_", self.val["basis_"]

    def display_hint(self):
        return None

class VerifiedU64Printer:
    """Pretty printer for verified_u64 type"""

    def __init__(self, val):
        self.val = val

    def to_string(self):
        try:
            value = int(self.val["basis_"]["basis_"]) & 0xFFFFFFFFFFFFFFFF
            return f"{value:,}"
        except Exception as e:
            return f"<invalid verified_u64: {e}>"

    def children(self):
        yield "basis_", self.val["basis_"]

    def display_hint(self):
        return None

class VerifiedU128Printer:
    """Pretty printer for verified_u128 type"""

    def __init__(self, val):
        self.val = val

    def to_string(self):
        try:
            inner = self.val["basis_"]["basis_"]
            low = int(inner["low"]) & 0xFFFFFFFFFFFFFFFF
            high = int(inner["high"]) & 0xFFFFFFFFFFFFFFFF
            value = (high << 64) | low
            return f"{value:,}"
        except Exception as e:
            return f"<invalid verified_u128: {e}>"

    def children(self):
        yield "basis_", self.val["basis_"]

    def display_hint(self):
        return None

class VerifiedBoundedUintPrinter:
    """Pretty printer for verified_bounded_integer types"""

    def __init__(self, val):
        self.val = val

    def to_string(self):
        try:
            type_name = str(self.val.type)

            # Extract Min and Max from the template parameters
            # Try verified_bounded_integer<Min, Max> first, then bounded_uint<Min, Max>
            match = re.search(r'(?:verified_bounded_integer|bounded_uint)<([^,]+),\s*([^>]+)>', type_name)
            if match:
                min_str = re.sub(r'[UuLl]+$', '', match.group(1).strip())
                max_str = re.sub(r'[UuLl]+$', '', match.group(2).strip())
            else:
                min_str = "?"
                max_str = "?"

            # basis_ is bounded_uint, which has basis_ (uN type),
            # which has basis_ (raw value)
            bounded = self.val["basis_"]
            inner = bounded["basis_"]["basis_"]

            # Check if this is a u128 (has low/high members)
            try:
                low = int(inner["low"]) & 0xFFFFFFFFFFFFFFFF
                high = int(inner["high"]) & 0xFFFFFFFFFFFFFFFF
                current = (high << 64) | low
            except gdb.error:
                byte_size = inner.type.sizeof
                mask = (1 << (byte_size * 8)) - 1
                current = int(inner) & mask

            return f"[{min_str}, {max_str}] {current:,}"
        except Exception as e:
            return f"<invalid verified_bounded_uint: {e}>"

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
    u128_pattern = re.compile(
        r"^(boost::safe_numbers::detail::unsigned_integer_basis<boost::safe_numbers::int128::uint128_t>|(\w+::)*u128)( &| \*)?$"
    )
    bounded_uint_pattern = re.compile(
        r"^boost::safe_numbers::bounded_uint<[^>]+>( &| \*)?$"
    )

    verified_u8_pattern = re.compile(
        r"^(boost::safe_numbers::detail::verified_type_basis<boost::safe_numbers::detail::unsigned_integer_basis<unsigned char>\s*>|(\w+::)*verified_u8)( &| \*)?$"
    )
    verified_u16_pattern = re.compile(
        r"^(boost::safe_numbers::detail::verified_type_basis<boost::safe_numbers::detail::unsigned_integer_basis<unsigned short>\s*>|(\w+::)*verified_u16)( &| \*)?$"
    )
    verified_u32_pattern = re.compile(
        r"^(boost::safe_numbers::detail::verified_type_basis<boost::safe_numbers::detail::unsigned_integer_basis<unsigned int>\s*>|(\w+::)*verified_u32)( &| \*)?$"
    )
    verified_u64_pattern = re.compile(
        r"^(boost::safe_numbers::detail::verified_type_basis<boost::safe_numbers::detail::unsigned_integer_basis<unsigned long>\s*>|boost::safe_numbers::detail::verified_type_basis<boost::safe_numbers::detail::unsigned_integer_basis<unsigned long long>\s*>|(\w+::)*verified_u64)( &| \*)?$"
    )
    verified_u128_pattern = re.compile(
        r"^(boost::safe_numbers::detail::verified_type_basis<boost::safe_numbers::detail::unsigned_integer_basis<boost::safe_numbers::int128::uint128_t>\s*>|(\w+::)*verified_u128)( &| \*)?$"
    )
    verified_bounded_uint_pattern = re.compile(
        r"^(boost::safe_numbers::detail::verified_type_basis<boost::safe_numbers::bounded_uint<[^>]+>\s*>|(\w+::)*verified_bounded_integer<[^>]+>)( &| \*)?$"
    )

    if u8_pattern.match(type_name):
        return U8Printer(val)
    if u16_pattern.match(type_name):
        return U16Printer(val)
    if u32_pattern.match(type_name):
        return U32Printer(val)
    if u64_pattern.match(type_name):
        return U64Printer(val)
    if u128_pattern.match(type_name):
        return U128Printer(val)
    if bounded_uint_pattern.match(type_name):
        return BoundedUintPrinter(val)
    if verified_u8_pattern.match(type_name):
        return VerifiedU8Printer(val)
    if verified_u16_pattern.match(type_name):
        return VerifiedU16Printer(val)
    if verified_u32_pattern.match(type_name):
        return VerifiedU32Printer(val)
    if verified_u64_pattern.match(type_name):
        return VerifiedU64Printer(val)
    if verified_u128_pattern.match(type_name):
        return VerifiedU128Printer(val)
    if verified_bounded_uint_pattern.match(type_name):
        return VerifiedBoundedUintPrinter(val)

    return None


def register_safe_numbers_printers(objfile=None):
    """Register the int128 pretty printers."""
    if objfile is None:
        objfile = gdb

    objfile.pretty_printers.append(lookup_safe_numbers_type)


# Auto-register when the script is sourced
register_safe_numbers_printers()
print("Safe_numbers pretty printers loaded successfully")
