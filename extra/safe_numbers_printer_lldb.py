# Copyright 2025 Matt Borland
# Distributed under the Boost Software License, Version 1.0.
# https://www.boost.org/LICENSE_1_0.txt

import lldb
import re

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
        type_name = val.GetType().GetName()

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

def __lldb_init_module(debugger, internal_dict):
    u8_pattern = r"^(const )?(boost::safe_numbers::detail::unsigned_integer_basis<unsigned char>|(\w+::)*u8)( &| \*)?$"
    u16_pattern = r"^(const )?(boost::safe_numbers::detail::unsigned_integer_basis<unsigned short>|(\w+::)*u16)( &| \*)?$"
    u32_pattern = r"^(const )?(boost::safe_numbers::detail::unsigned_integer_basis<unsigned int>|(\w+::)*u32)( &| \*)?$"
    u64_pattern = r"^(const )?(boost::safe_numbers::detail::unsigned_integer_basis<unsigned long>|boost::safe_numbers::detail::unsigned_integer_basis<unsigned long long>|(\w+::)*u64)( &| \*)?$"
    u128_pattern = r"^(const )?(boost::safe_numbers::detail::unsigned_integer_basis<boost::safe_numbers::int128::uint128_t>|(\w+::)*u128)( &| \*)?$"
    bounded_uint_pattern = r"^(const )?boost::safe_numbers::bounded_uint<[^>]+>( &| \*)?$"

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
