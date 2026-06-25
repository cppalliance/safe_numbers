# Copyright 2025 Matt Borland
# Distributed under the Boost Software License, Version 1.0.
# https://www.boost.org/LICENSE_1_0.txt

import gdb
import gdb.printing
import re
import struct

def _shortest_float(x, fmt):
    """
    Render a float with the fewest digits that still round-trips.
    fmt is 'f' for 32-bit or 'd' for 64-bit values. Fixed notation is preferred
    over the exponent form when it is no longer, so 10.0 shows as '10' not '1e+01'.
    """
    if x != x:
        return "nan"
    if x == float("inf"):
        return "inf"
    if x == float("-inf"):
        return "-inf"

    try:
        packed = struct.pack('<' + fmt, x)
    except (OverflowError, struct.error):
        return repr(x)

    limit = 10 if fmt == 'f' else 18
    for prec in range(limit + 1):
        best = None
        for candidate in (f"{x:.{prec}f}", f"{x:.{prec}g}"):
            try:
                if struct.pack('<' + fmt, float(candidate)) != packed:
                    continue
            except (ValueError, OverflowError, struct.error):
                continue
            if '.' in candidate and 'e' not in candidate and 'E' not in candidate:
                candidate = candidate.rstrip('0').rstrip('.')
            if best is None or len(candidate) < len(best):
                best = candidate
        if best is not None:
            return best

    return repr(x)

def _clean_float_bound(text, fmt):
    """
    Turn a floating-point template argument as spelled by the debugger into a
    clean number. GDB renders bounds like '(float)-[1.0e+1]' and LLDB like
    '-1.000000e+01'; both reduce to '-10'.
    """
    s = text.strip()
    s = re.sub(r'^\((?:float|double|long double)\)\s*', '', s).strip()

    # GDB brackets the magnitude and keeps the sign outside: -[1.0e+1]
    bracket = re.match(r'^([+-]?)\s*\[(.*)\]$', s)
    if bracket:
        s = bracket.group(1) + bracket.group(2)

    try:
        return _shortest_float(float(s), fmt)
    except ValueError:
        return text.strip()

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

class I8Printer:
    """Pretty printer for i8 type"""

    def __init__(self, val):
        self.val = val

    def to_string(self):
        try:
            return f"{int(self.val['basis_']):,}"
        except Exception as e:
            return f"<invalid i8: {e}>"

    def children(self):
        yield "basis_", self.val["basis_"]

    def display_hint(self):
        return None

class I16Printer:
    """Pretty printer for i16 type"""

    def __init__(self, val):
        self.val = val

    def to_string(self):
        try:
            return f"{int(self.val['basis_']):,}"
        except Exception as e:
            return f"<invalid i16: {e}>"

    def children(self):
        yield "basis_", self.val["basis_"]

    def display_hint(self):
        return None

class I32Printer:
    """Pretty printer for i32 type"""

    def __init__(self, val):
        self.val = val

    def to_string(self):
        try:
            return f"{int(self.val['basis_']):,}"
        except Exception as e:
            return f"<invalid i32: {e}>"

    def children(self):
        yield "basis_", self.val["basis_"]

    def display_hint(self):
        return None

class I64Printer:
    """Pretty printer for i64 type"""

    def __init__(self, val):
        self.val = val

    def to_string(self):
        try:
            return f"{int(self.val['basis_']):,}"
        except Exception as e:
            return f"<invalid i64: {e}>"

    def children(self):
        yield "basis_", self.val["basis_"]

    def display_hint(self):
        return None

class I128Printer:
    """Pretty printer for i128 type"""

    def __init__(self, val):
        self.val = val

    def to_string(self):
        try:
            basis = self.val["basis_"]
            low = int(basis["low"]) & 0xFFFFFFFFFFFFFFFF
            high = int(basis["high"]) & 0xFFFFFFFFFFFFFFFF
            value = (high << 64) | low
            if value >= (1 << 127):
                value -= (1 << 128)
            return f"{value:,}"
        except Exception as e:
            return f"<invalid i128: {e}>"

    def children(self):
        yield "basis_", self.val["basis_"]

    def display_hint(self):
        return None

class F32Printer:
    """Pretty printer for f32 type"""

    def __init__(self, val):
        self.val = val

    def to_string(self):
        try:
            return _shortest_float(float(self.val["basis_"]), 'f')
        except Exception as e:
            return f"<invalid f32: {e}>"

    def children(self):
        yield "basis_", self.val["basis_"]

    def display_hint(self):
        return None

class F64Printer:
    """Pretty printer for f64 type"""

    def __init__(self, val):
        self.val = val

    def to_string(self):
        try:
            return _shortest_float(float(self.val["basis_"]), 'd')
        except Exception as e:
            return f"<invalid f64: {e}>"

    def children(self):
        yield "basis_", self.val["basis_"]

    def display_hint(self):
        return None

class BoundedIntPrinter:
    """Pretty printer for bounded_int types"""

    def __init__(self, val):
        self.val = val

    def to_string(self):
        try:
            type_name = str(self.val.type)

            # Extract Min and Max from template parameters
            match = re.search(r'bounded_int<([^,]+),\s*([^>]+)>', type_name)
            if match:
                min_str = match.group(1).strip()
                max_str = match.group(2).strip()
            else:
                min_str = "?"
                max_str = "?"

            # basis_ is a safe number type (i8/i16/i32/i64/i128),
            # which itself has a basis_ member with the raw value
            basis = self.val["basis_"]
            inner = basis["basis_"]

            # Check if this is an i128 (has low/high members)
            try:
                low = int(inner["low"]) & 0xFFFFFFFFFFFFFFFF
                high = int(inner["high"]) & 0xFFFFFFFFFFFFFFFF
                current = (high << 64) | low
                if current >= (1 << 127):
                    current -= (1 << 128)
            except gdb.error:
                current = int(inner)

            return f"[{min_str}, {max_str}] {current:,}"
        except Exception as e:
            return f"<invalid bounded_int: {e}>"

    def children(self):
        yield "basis_", self.val["basis_"]

    def display_hint(self):
        return None

class BoundedFloatPrinter:
    """Pretty printer for bounded_float types"""

    def __init__(self, val):
        self.val = val

    def to_string(self):
        try:
            # basis_ is a safe float type (f32/f64), whose own basis_ is the raw value
            basis = self.val["basis_"]
            inner = basis["basis_"]
            fmt = 'f' if inner.type.sizeof == 4 else 'd'

            type_name = str(self.val.type)

            # Extract Min and Max from template parameters
            match = re.search(r'bounded_float<(.+),\s*(.+)>', type_name)
            if match:
                min_str = _clean_float_bound(match.group(1), fmt)
                max_str = _clean_float_bound(match.group(2), fmt)
            else:
                min_str = "?"
                max_str = "?"

            current = _shortest_float(float(inner), fmt)

            return f"[{min_str}, {max_str}] {current}"
        except Exception as e:
            return f"<invalid bounded_float: {e}>"

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
    i8_pattern = re.compile(
        r"^(boost::safe_numbers::detail::signed_integer_basis<signed char>|(\w+::)*i8)( &| \*)?$"
    )
    i16_pattern = re.compile(
        r"^(boost::safe_numbers::detail::signed_integer_basis<short>|(\w+::)*i16)( &| \*)?$"
    )
    i32_pattern = re.compile(
        r"^(boost::safe_numbers::detail::signed_integer_basis<int>|(\w+::)*i32)( &| \*)?$"
    )
    i64_pattern = re.compile(
        r"^(boost::safe_numbers::detail::signed_integer_basis<long>|boost::safe_numbers::detail::signed_integer_basis<long long>|(\w+::)*i64)( &| \*)?$"
    )
    i128_pattern = re.compile(
        r"^(boost::safe_numbers::detail::signed_integer_basis<boost::safe_numbers::int128::int128_t>|(\w+::)*i128)( &| \*)?$"
    )
    bounded_int_pattern = re.compile(
        r"^boost::safe_numbers::bounded_int<[^>]+>( &| \*)?$"
    )
    f32_pattern = re.compile(
        r"^(boost::safe_numbers::detail::float_basis<float>|(\w+::)*f32)( &| \*)?$"
    )
    f64_pattern = re.compile(
        r"^(boost::safe_numbers::detail::float_basis<double>|(\w+::)*f64)( &| \*)?$"
    )
    bounded_float_pattern = re.compile(
        r"^boost::safe_numbers::bounded_float<.+>( &| \*)?$"
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
    if i8_pattern.match(type_name):
        return I8Printer(val)
    if i16_pattern.match(type_name):
        return I16Printer(val)
    if i32_pattern.match(type_name):
        return I32Printer(val)
    if i64_pattern.match(type_name):
        return I64Printer(val)
    if i128_pattern.match(type_name):
        return I128Printer(val)
    if bounded_int_pattern.match(type_name):
        return BoundedIntPrinter(val)
    if f32_pattern.match(type_name):
        return F32Printer(val)
    if f64_pattern.match(type_name):
        return F64Printer(val)
    if bounded_float_pattern.match(type_name):
        return BoundedFloatPrinter(val)

    return None


def register_safe_numbers_printers(objfile=None):
    """Register the int128 pretty printers."""
    if objfile is None:
        objfile = gdb

    objfile.pretty_printers.append(lookup_safe_numbers_type)


# Auto-register when the script is sourced
register_safe_numbers_printers()
print("Safe_numbers pretty printers loaded successfully")
