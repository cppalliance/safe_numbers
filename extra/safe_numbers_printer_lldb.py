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

def verified_u8_summary(valobj, internal_dict):
    """
    Custom summary for verified_u8 type
    Displays as decimal (base 10), matching u8 display
    """
    try:
        val = valobj.GetNonSyntheticValue()
        basis = val.GetChildMemberWithName("basis_")
        basis_value = basis.GetChildMemberWithName("basis_").GetValueAsUnsigned()

        return f"{basis_value:,}"
    except Exception as e:
        return f"<invalid verified_u8: {e}>"

def verified_u16_summary(valobj, internal_dict):
    """
    Custom summary for verified_u16 type
    Displays as decimal (base 10), matching u16 display
    """
    try:
        val = valobj.GetNonSyntheticValue()
        basis = val.GetChildMemberWithName("basis_")
        basis_value = basis.GetChildMemberWithName("basis_").GetValueAsUnsigned()

        return f"{basis_value:,}"
    except Exception as e:
        return f"<invalid verified_u16: {e}>"

def verified_u32_summary(valobj, internal_dict):
    """
    Custom summary for verified_u32 type
    Displays as decimal (base 10), matching u32 display
    """
    try:
        val = valobj.GetNonSyntheticValue()
        basis = val.GetChildMemberWithName("basis_")
        basis_value = basis.GetChildMemberWithName("basis_").GetValueAsUnsigned()

        return f"{basis_value:,}"
    except Exception as e:
        return f"<invalid verified_u32: {e}>"

def verified_u64_summary(valobj, internal_dict):
    """
    Custom summary for verified_u64 type
    Displays as decimal (base 10), matching u64 display
    """
    try:
        val = valobj.GetNonSyntheticValue()
        basis = val.GetChildMemberWithName("basis_")
        basis_value = basis.GetChildMemberWithName("basis_").GetValueAsUnsigned()

        return f"{basis_value:,}"
    except Exception as e:
        return f"<invalid verified_u64: {e}>"

def verified_u128_summary(valobj, internal_dict):
    """
    Custom summary for verified_u128 type
    Displays as decimal (base 10), matching u128 display
    """
    try:
        val = valobj.GetNonSyntheticValue()
        basis = val.GetChildMemberWithName("basis_")
        inner = basis.GetChildMemberWithName("basis_")
        low = inner.GetChildMemberWithName("low").GetValueAsUnsigned()
        high = inner.GetChildMemberWithName("high").GetValueAsUnsigned()
        value = (high << 64) | low

        return f"{value:,}"
    except Exception as e:
        return f"<invalid verified_u128: {e}>"

def verified_bounded_uint_summary(valobj, internal_dict):
    """
    Custom summary for verified_bounded_integer types
    Displays as [Min, Max] Current, matching bounded_uint display
    """
    try:
        val = valobj.GetNonSyntheticValue()
        type_name = val.GetType().GetName()

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
        bounded = val.GetChildMemberWithName("basis_")
        inner_type = bounded.GetChildMemberWithName("basis_")
        inner_basis = inner_type.GetChildMemberWithName("basis_")

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
        return f"<invalid verified_bounded_uint: {e}>"

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

    verified_u8_pattern = r"^(const )?(boost::safe_numbers::detail::verified_type_basis<boost::safe_numbers::detail::unsigned_integer_basis<unsigned char> ?>|([a-zA-Z0-9_]+::)*verified_u8)( &| \*)?$"
    verified_u16_pattern = r"^(const )?(boost::safe_numbers::detail::verified_type_basis<boost::safe_numbers::detail::unsigned_integer_basis<unsigned short> ?>|([a-zA-Z0-9_]+::)*verified_u16)( &| \*)?$"
    verified_u32_pattern = r"^(const )?(boost::safe_numbers::detail::verified_type_basis<boost::safe_numbers::detail::unsigned_integer_basis<unsigned int> ?>|([a-zA-Z0-9_]+::)*verified_u32)( &| \*)?$"
    verified_u64_pattern = r"^(const )?(boost::safe_numbers::detail::verified_type_basis<boost::safe_numbers::detail::unsigned_integer_basis<unsigned long> ?>|boost::safe_numbers::detail::verified_type_basis<boost::safe_numbers::detail::unsigned_integer_basis<unsigned long long> ?>|([a-zA-Z0-9_]+::)*verified_u64)( &| \*)?$"
    verified_u128_pattern = r"^(const )?(boost::safe_numbers::detail::verified_type_basis<boost::safe_numbers::detail::unsigned_integer_basis<boost::safe_numbers::int128::uint128_t> ?>|([a-zA-Z0-9_]+::)*verified_u128)( &| \*)?$"
    verified_bounded_uint_pattern = r"^(const )?(boost::safe_numbers::detail::verified_type_basis<boost::safe_numbers::bounded_uint<[^>]+> ?>|([a-zA-Z0-9_]+::)*verified_bounded_integer<[^>]+>)( &| \*)?$"

    debugger.HandleCommand(
        f'type summary add -x "{verified_u8_pattern}" -e -F safe_numbers_printer_lldb.verified_u8_summary'
    )
    debugger.HandleCommand(
        f'type synthetic add -x "{verified_u8_pattern}" -l safe_numbers_printer_lldb.VerifiedTypeSyntheticProvider'
    )

    print("verified_u8 printer loaded successfully")

    debugger.HandleCommand(
        f'type summary add -x "{verified_u16_pattern}" -e -F safe_numbers_printer_lldb.verified_u16_summary'
    )
    debugger.HandleCommand(
        f'type synthetic add -x "{verified_u16_pattern}" -l safe_numbers_printer_lldb.VerifiedTypeSyntheticProvider'
    )

    print("verified_u16 printer loaded successfully")

    debugger.HandleCommand(
        f'type summary add -x "{verified_u32_pattern}" -e -F safe_numbers_printer_lldb.verified_u32_summary'
    )
    debugger.HandleCommand(
        f'type synthetic add -x "{verified_u32_pattern}" -l safe_numbers_printer_lldb.VerifiedTypeSyntheticProvider'
    )

    print("verified_u32 printer loaded successfully")

    debugger.HandleCommand(
        f'type summary add -x "{verified_u64_pattern}" -e -F safe_numbers_printer_lldb.verified_u64_summary'
    )
    debugger.HandleCommand(
        f'type synthetic add -x "{verified_u64_pattern}" -l safe_numbers_printer_lldb.VerifiedTypeSyntheticProvider'
    )

    print("verified_u64 printer loaded successfully")

    debugger.HandleCommand(
        f'type summary add -x "{verified_u128_pattern}" -e -F safe_numbers_printer_lldb.verified_u128_summary'
    )
    debugger.HandleCommand(
        f'type synthetic add -x "{verified_u128_pattern}" -l safe_numbers_printer_lldb.VerifiedTypeSyntheticProvider'
    )

    print("verified_u128 printer loaded successfully")

    debugger.HandleCommand(
        f'type summary add -x "{verified_bounded_uint_pattern}" -e -F safe_numbers_printer_lldb.verified_bounded_uint_summary'
    )
    debugger.HandleCommand(
        f'type synthetic add -x "{verified_bounded_uint_pattern}" -l safe_numbers_printer_lldb.VerifiedTypeSyntheticProvider'
    )

    print("verified_bounded_uint printer loaded successfully")


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
