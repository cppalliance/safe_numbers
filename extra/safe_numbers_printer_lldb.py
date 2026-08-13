# Copyright 2025 Matt Borland
# Distributed under the Boost Software License, Version 1.0.
# https://www.boost.org/LICENSE_1_0.txt

import lldb
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
    clean number. LLDB renders bounds like '-1.000000e+01' and GDB like
    '(float)-[1.0e+1]'; both reduce to '-10'.
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

def _read_float(sbvalue):
    """Read an SBValue of float/double as (python_float, fmt_char)."""
    err = lldb.SBError()
    data = sbvalue.GetData()
    if sbvalue.GetByteSize() == 4:
        return data.GetFloat(err, 0), 'f'
    return data.GetDouble(err, 0), 'd'

def _template_name(sbtype, keyword):
    """
    Return the type-name spelling that contains keyword. A `using` typedef
    renders as the alias name (e.g. 'temperature'), which hides the template
    bounds, so fall back to the canonical type when the alias does not show them.
    """
    name = sbtype.GetName() or ""
    if keyword not in name:
        name = sbtype.GetCanonicalType().GetName() or name
    return name

def _int_bound(text):
    """
    Return an integer-literal bound, or '?' when the spelling is not a plain
    literal. A bound that needs more than 64 bits is a 128-bit non-type template
    argument, which the debugger renders as a type name rather than a number.
    """
    s = text.strip()
    return s if re.fullmatch(r'[+-]?[0-9]+', s) else "?"

def u8_summary(valobj, internal_dict):
    """
    Custom summary for u8 type
    Displays as decimal (base 10)
    """
    try:
        val = valobj.GetNonSyntheticValue()
        basis_value = val.GetChildMemberWithName("basis_").GetValueAsUnsigned()

        return f"{basis_value:,}"
    except Exception as e:
        return f"<invalid u8: {e}>"

def u16_summary(valobj, internal_dict):
    """
    Custom summary for u16 type
    Displays as decimal (base 10)
    """
    try:
        val = valobj.GetNonSyntheticValue()
        basis_value = val.GetChildMemberWithName("basis_").GetValueAsUnsigned()

        return f"{basis_value:,}"
    except Exception as e:
        return f"<invalid u16: {e}>"

def u32_summary(valobj, internal_dict):
    """
    Custom summary for u32 type
    Displays as decimal (base 10)
    """
    try:
        val = valobj.GetNonSyntheticValue()
        basis_value = val.GetChildMemberWithName("basis_").GetValueAsUnsigned()

        return f"{basis_value:,}"
    except Exception as e:
        return f"<invalid u32: {e}>"

def u64_summary(valobj, internal_dict):
    """
    Custom summary for u64 type
    Displays as decimal (base 10)
    """
    try:
        val = valobj.GetNonSyntheticValue()
        basis_value = val.GetChildMemberWithName("basis_").GetValueAsUnsigned()

        return f"{basis_value:,}"
    except Exception as e:
        return f"<invalid u64: {e}>"

def u128_summary(valobj, internal_dict):
    """
    Custom summary for u128 type
    Displays as decimal (base 10)
    """
    try:
        val = valobj.GetNonSyntheticValue()
        basis = val.GetChildMemberWithName("basis_")
        low = basis.GetChildMemberWithName("low").GetValueAsUnsigned()
        high = basis.GetChildMemberWithName("high").GetValueAsUnsigned()
        value = (high << 64) | low

        return f"{value:,}"
    except Exception as e:
        return f"<invalid u128: {e}>"

def bounded_uint_summary(valobj, internal_dict):
    """
    Custom summary for bounded_uint types
    Displays as [Min, Max] Current
    """
    try:
        val = valobj.GetNonSyntheticValue()
        type_name = _template_name(val.GetType(), "bounded_uint<")

        # Extract Min and Max from template parameters
        match = re.search(r'bounded_uint<([^,]+),\s*([^>]+)>', type_name)
        if match:
            min_str = _int_bound(match.group(1))
            max_str = _int_bound(match.group(2))
        else:
            min_str = "?"
            max_str = "?"

        # basis_ is a safe number type (u8/u16/u32/u64/u128),
        # which itself has a basis_ member with the raw value
        basis = val.GetChildMemberWithName("basis_")
        inner_basis = basis.GetChildMemberWithName("basis_")

        # Check if the inner basis is a uint128_t (has low/high members)
        low_member = inner_basis.GetChildMemberWithName("low")
        if low_member.IsValid():
            low = low_member.GetValueAsUnsigned()
            high = inner_basis.GetChildMemberWithName("high").GetValueAsUnsigned()
            current = (high << 64) | low
        else:
            current = inner_basis.GetValueAsUnsigned()

        return f"[{min_str}, {max_str}] {current:,}"
    except Exception as e:
        return f"<invalid bounded_uint: {e}>"

def i8_summary(valobj, internal_dict):
    """
    Custom summary for i8 type
    Displays as decimal (base 10)
    """
    try:
        val = valobj.GetNonSyntheticValue()
        basis_value = val.GetChildMemberWithName("basis_").GetValueAsSigned()

        return f"{basis_value:,}"
    except Exception as e:
        return f"<invalid i8: {e}>"

def i16_summary(valobj, internal_dict):
    """
    Custom summary for i16 type
    Displays as decimal (base 10)
    """
    try:
        val = valobj.GetNonSyntheticValue()
        basis_value = val.GetChildMemberWithName("basis_").GetValueAsSigned()

        return f"{basis_value:,}"
    except Exception as e:
        return f"<invalid i16: {e}>"

def i32_summary(valobj, internal_dict):
    """
    Custom summary for i32 type
    Displays as decimal (base 10)
    """
    try:
        val = valobj.GetNonSyntheticValue()
        basis_value = val.GetChildMemberWithName("basis_").GetValueAsSigned()

        return f"{basis_value:,}"
    except Exception as e:
        return f"<invalid i32: {e}>"

def i64_summary(valobj, internal_dict):
    """
    Custom summary for i64 type
    Displays as decimal (base 10)
    """
    try:
        val = valobj.GetNonSyntheticValue()
        basis_value = val.GetChildMemberWithName("basis_").GetValueAsSigned()

        return f"{basis_value:,}"
    except Exception as e:
        return f"<invalid i64: {e}>"

def i128_summary(valobj, internal_dict):
    """
    Custom summary for i128 type
    Displays as decimal (base 10)
    """
    try:
        val = valobj.GetNonSyntheticValue()
        basis = val.GetChildMemberWithName("basis_")
        low = basis.GetChildMemberWithName("low").GetValueAsUnsigned()
        high = basis.GetChildMemberWithName("high").GetValueAsUnsigned() & 0xFFFFFFFFFFFFFFFF
        value = (high << 64) | low
        if value >= (1 << 127):
            value -= (1 << 128)

        return f"{value:,}"
    except Exception as e:
        return f"<invalid i128: {e}>"

def f32_summary(valobj, internal_dict):
    """
    Custom summary for f32 type
    """
    try:
        val = valobj.GetNonSyntheticValue()
        value, fmt = _read_float(val.GetChildMemberWithName("basis_"))

        return _shortest_float(value, fmt)
    except Exception as e:
        return f"<invalid f32: {e}>"

def f64_summary(valobj, internal_dict):
    """
    Custom summary for f64 type
    """
    try:
        val = valobj.GetNonSyntheticValue()
        value, fmt = _read_float(val.GetChildMemberWithName("basis_"))

        return _shortest_float(value, fmt)
    except Exception as e:
        return f"<invalid f64: {e}>"

def bounded_int_summary(valobj, internal_dict):
    """
    Custom summary for bounded_int types
    Displays as [Min, Max] Current
    """
    try:
        val = valobj.GetNonSyntheticValue()
        type_name = _template_name(val.GetType(), "bounded_int<")

        # Extract Min and Max from template parameters
        match = re.search(r'bounded_int<([^,]+),\s*([^>]+)>', type_name)
        if match:
            min_str = _int_bound(match.group(1))
            max_str = _int_bound(match.group(2))
        else:
            min_str = "?"
            max_str = "?"

        # basis_ is a safe number type (i8/i16/i32/i64/i128),
        # which itself has a basis_ member with the raw value
        basis = val.GetChildMemberWithName("basis_")
        inner_basis = basis.GetChildMemberWithName("basis_")

        # Check if the inner basis is an int128_t (has low/high members)
        low_member = inner_basis.GetChildMemberWithName("low")
        if low_member.IsValid():
            low = low_member.GetValueAsUnsigned()
            high = inner_basis.GetChildMemberWithName("high").GetValueAsUnsigned() & 0xFFFFFFFFFFFFFFFF
            current = (high << 64) | low
            if current >= (1 << 127):
                current -= (1 << 128)
        else:
            current = inner_basis.GetValueAsSigned()

        return f"[{min_str}, {max_str}] {current:,}"
    except Exception as e:
        return f"<invalid bounded_int: {e}>"

def bounded_float_summary(valobj, internal_dict):
    """
    Custom summary for bounded_float types
    Displays as [Min, Max] Current
    """
    try:
        val = valobj.GetNonSyntheticValue()

        # basis_ is a safe float type (f32/f64), whose own basis_ is the raw value
        basis = val.GetChildMemberWithName("basis_")
        inner_basis = basis.GetChildMemberWithName("basis_")
        value, fmt = _read_float(inner_basis)

        type_name = _template_name(val.GetType(), "bounded_float<")

        # Extract Min and Max from template parameters
        match = re.search(r'bounded_float<(.+),\s*(.+)>', type_name)
        if match:
            min_str = _clean_float_bound(match.group(1), fmt)
            max_str = _clean_float_bound(match.group(2), fmt)
        else:
            min_str = "?"
            max_str = "?"

        current = _shortest_float(value, fmt)

        return f"[{min_str}, {max_str}] {current}"
    except Exception as e:
        return f"<invalid bounded_float: {e}>"

def __lldb_init_module(debugger, internal_dict):
    # LLDB matches these with llvm::Regex, which mishandles "\w" inside a
    # quantified group under "^"/"$" anchors (it silently fails to match the
    # alias, e.g. boost::safe_numbers::u128). The explicit "[A-Za-z0-9_]"
    # class is equivalent and matches reliably, so use it instead of "\w".
    u8_pattern = r"^(const )?(boost::safe_numbers::detail::unsigned_integer_basis<unsigned char(, [^>]+)?>|([A-Za-z0-9_]+::)*(sat_|strict_)?u8)( &| \*)?$"
    u16_pattern = r"^(const )?(boost::safe_numbers::detail::unsigned_integer_basis<unsigned short(, [^>]+)?>|([A-Za-z0-9_]+::)*(sat_|strict_)?u16)( &| \*)?$"
    u32_pattern = r"^(const )?(boost::safe_numbers::detail::unsigned_integer_basis<unsigned int(, [^>]+)?>|([A-Za-z0-9_]+::)*(sat_|strict_)?u32)( &| \*)?$"
    u64_pattern = r"^(const )?(boost::safe_numbers::detail::unsigned_integer_basis<unsigned long(, [^>]+)?>|boost::safe_numbers::detail::unsigned_integer_basis<unsigned long long(, [^>]+)?>|([A-Za-z0-9_]+::)*(sat_|strict_)?u64)( &| \*)?$"
    u128_pattern = r"^(const )?(boost::safe_numbers::detail::unsigned_integer_basis<boost::int128::uint128_t(, [^>]+)?>|([A-Za-z0-9_]+::)*(sat_|strict_)?u128)( &| \*)?$"
    bounded_uint_pattern = r"^(const )?boost::safe_numbers::bounded_uint<[^>]+>( &| \*)?$"
    i8_pattern = r"^(const )?(boost::safe_numbers::detail::signed_integer_basis<signed char(, [^>]+)?>|([A-Za-z0-9_]+::)*(sat_|strict_)?i8)( &| \*)?$"
    i16_pattern = r"^(const )?(boost::safe_numbers::detail::signed_integer_basis<short(, [^>]+)?>|([A-Za-z0-9_]+::)*(sat_|strict_)?i16)( &| \*)?$"
    i32_pattern = r"^(const )?(boost::safe_numbers::detail::signed_integer_basis<int(, [^>]+)?>|([A-Za-z0-9_]+::)*(sat_|strict_)?i32)( &| \*)?$"
    i64_pattern = r"^(const )?(boost::safe_numbers::detail::signed_integer_basis<long(, [^>]+)?>|boost::safe_numbers::detail::signed_integer_basis<long long(, [^>]+)?>|([A-Za-z0-9_]+::)*(sat_|strict_)?i64)( &| \*)?$"
    i128_pattern = r"^(const )?(boost::safe_numbers::detail::signed_integer_basis<boost::int128::int128_t(, [^>]+)?>|([A-Za-z0-9_]+::)*(sat_|strict_)?i128)( &| \*)?$"
    bounded_int_pattern = r"^(const )?boost::safe_numbers::bounded_int<[^>]+>( &| \*)?$"
    f32_pattern = r"^(const )?(boost::safe_numbers::detail::float_basis<float(, [^>]+)?>|([A-Za-z0-9_]+::)*(sat_)?f32)( &| \*)?$"
    f64_pattern = r"^(const )?(boost::safe_numbers::detail::float_basis<double(, [^>]+)?>|([A-Za-z0-9_]+::)*(sat_)?f64)( &| \*)?$"
    bounded_float_pattern = r"^(const )?boost::safe_numbers::bounded_float<.+>( &| \*)?$"

    debugger.HandleCommand(
        f'type summary add -x "{u8_pattern}" -e -F safe_numbers_printer_lldb.u8_summary'
    )
    debugger.HandleCommand(
        f'type synthetic add -x "{u8_pattern}" -l safe_numbers_printer_lldb.UintSyntheticProvider'
    )

    print("u8 printer loaded successfully")

    debugger.HandleCommand(
        f'type summary add -x "{u16_pattern}" -e -F safe_numbers_printer_lldb.u16_summary'
    )
    debugger.HandleCommand(
        f'type synthetic add -x "{u16_pattern}" -l safe_numbers_printer_lldb.UintSyntheticProvider'
    )

    print("u16 printer loaded successfully")

    debugger.HandleCommand(
        f'type summary add -x "{u32_pattern}" -e -F safe_numbers_printer_lldb.u32_summary'
    )
    debugger.HandleCommand(
        f'type synthetic add -x "{u32_pattern}" -l safe_numbers_printer_lldb.UintSyntheticProvider'
    )

    print("u32 printer loaded successfully")

    debugger.HandleCommand(
        f'type summary add -x "{u64_pattern}" -e -F safe_numbers_printer_lldb.u64_summary'
    )
    debugger.HandleCommand(
        f'type synthetic add -x "{u64_pattern}" -l safe_numbers_printer_lldb.UintSyntheticProvider'
    )

    print("u64 printer loaded successfully")

    debugger.HandleCommand(
        f'type summary add -x "{u128_pattern}" -e -F safe_numbers_printer_lldb.u128_summary'
    )
    debugger.HandleCommand(
        f'type synthetic add -x "{u128_pattern}" -l safe_numbers_printer_lldb.U128SyntheticProvider'
    )

    print("u128 printer loaded successfully")

    debugger.HandleCommand(
        f'type summary add -x "{bounded_uint_pattern}" -e -F safe_numbers_printer_lldb.bounded_uint_summary'
    )
    debugger.HandleCommand(
        f'type synthetic add -x "{bounded_uint_pattern}" -l safe_numbers_printer_lldb.BoundedUintSyntheticProvider'
    )

    print("bounded_uint printer loaded successfully")

    debugger.HandleCommand(
        f'type summary add -x "{i8_pattern}" -e -F safe_numbers_printer_lldb.i8_summary'
    )
    debugger.HandleCommand(
        f'type synthetic add -x "{i8_pattern}" -l safe_numbers_printer_lldb.BasisSyntheticProvider'
    )

    print("i8 printer loaded successfully")

    debugger.HandleCommand(
        f'type summary add -x "{i16_pattern}" -e -F safe_numbers_printer_lldb.i16_summary'
    )
    debugger.HandleCommand(
        f'type synthetic add -x "{i16_pattern}" -l safe_numbers_printer_lldb.BasisSyntheticProvider'
    )

    print("i16 printer loaded successfully")

    debugger.HandleCommand(
        f'type summary add -x "{i32_pattern}" -e -F safe_numbers_printer_lldb.i32_summary'
    )
    debugger.HandleCommand(
        f'type synthetic add -x "{i32_pattern}" -l safe_numbers_printer_lldb.BasisSyntheticProvider'
    )

    print("i32 printer loaded successfully")

    debugger.HandleCommand(
        f'type summary add -x "{i64_pattern}" -e -F safe_numbers_printer_lldb.i64_summary'
    )
    debugger.HandleCommand(
        f'type synthetic add -x "{i64_pattern}" -l safe_numbers_printer_lldb.BasisSyntheticProvider'
    )

    print("i64 printer loaded successfully")

    debugger.HandleCommand(
        f'type summary add -x "{i128_pattern}" -e -F safe_numbers_printer_lldb.i128_summary'
    )
    debugger.HandleCommand(
        f'type synthetic add -x "{i128_pattern}" -l safe_numbers_printer_lldb.BasisSyntheticProvider'
    )

    print("i128 printer loaded successfully")

    debugger.HandleCommand(
        f'type summary add -x "{bounded_int_pattern}" -e -F safe_numbers_printer_lldb.bounded_int_summary'
    )
    debugger.HandleCommand(
        f'type synthetic add -x "{bounded_int_pattern}" -l safe_numbers_printer_lldb.BasisSyntheticProvider'
    )

    print("bounded_int printer loaded successfully")

    debugger.HandleCommand(
        f'type summary add -x "{f32_pattern}" -e -F safe_numbers_printer_lldb.f32_summary'
    )
    debugger.HandleCommand(
        f'type synthetic add -x "{f32_pattern}" -l safe_numbers_printer_lldb.BasisSyntheticProvider'
    )

    print("f32 printer loaded successfully")

    debugger.HandleCommand(
        f'type summary add -x "{f64_pattern}" -e -F safe_numbers_printer_lldb.f64_summary'
    )
    debugger.HandleCommand(
        f'type synthetic add -x "{f64_pattern}" -l safe_numbers_printer_lldb.BasisSyntheticProvider'
    )

    print("f64 printer loaded successfully")

    debugger.HandleCommand(
        f'type summary add -x "{bounded_float_pattern}" -e -F safe_numbers_printer_lldb.bounded_float_summary'
    )
    debugger.HandleCommand(
        f'type synthetic add -x "{bounded_float_pattern}" -l safe_numbers_printer_lldb.BasisSyntheticProvider'
    )

    print("bounded_float printer loaded successfully")


class BasisSyntheticProvider:
    def __init__(self, valobj, internal_dict):
        self.valobj = valobj

    def num_children(self):
        return 1

    def get_child_index(self, name):
        if name == "basis_":
            return 0
        return -1

    def get_child_at_index(self, index):
        if index == 0:
            return self.valobj.GetChildMemberWithName("basis_")
        return None

    def update(self):
        pass

    def has_children(self):
        return True


class UintSyntheticProvider:
    def __init__(self, valobj, internal_dict):
        self.valobj = valobj

    def num_children(self):
        return 1

    def get_child_index(self, name):
        if name == "basis_":
            return 0
        return -1

    def get_child_at_index(self, index):
        if index == 0:
            return self.valobj.GetChildMemberWithName("basis_")
        return None

    def update(self):
        pass

    def has_children(self):
        return True


class U128SyntheticProvider:
    def __init__(self, valobj, internal_dict):
        self.valobj = valobj

    def num_children(self):
        return 1

    def get_child_index(self, name):
        if name == "basis_":
            return 0
        return -1

    def get_child_at_index(self, index):
        if index == 0:
            return self.valobj.GetChildMemberWithName("basis_")
        return None

    def update(self):
        pass

    def has_children(self):
        return True


class BoundedUintSyntheticProvider:
    def __init__(self, valobj, internal_dict):
        self.valobj = valobj

    def num_children(self):
        return 1

    def get_child_index(self, name):
        if name == "basis_":
            return 0
        return -1

    def get_child_at_index(self, index):
        if index == 0:
            return self.valobj.GetChildMemberWithName("basis_")
        return None

    def update(self):
        pass

    def has_children(self):
        return True


class VerifiedTypeSyntheticProvider:
    def __init__(self, valobj, internal_dict):
        self.valobj = valobj

    def num_children(self):
        return 1

    def get_child_index(self, name):
        if name == "basis_":
            return 0
        return -1

    def get_child_at_index(self, index):
        if index == 0:
            return self.valobj.GetChildMemberWithName("basis_")
        return None

    def update(self):
        pass

    def has_children(self):
        return True
