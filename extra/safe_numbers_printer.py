# safe_numbers_printer.py

import lldb

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

def __lldb_init_module(debugger, internal_dict):
    u8_pattern = r"^(const )?(boost::safe_numbers::detail::unsigned_integer_basis<unsigned char>|(\w+::)*u8)( &| \*)?$"
    u16_pattern = r"^(const )?(boost::safe_numbers::detail::unsigned_integer_basis<unsigned short>|(\w+::)*u16)( &| \*)?$"
    u32_pattern = r"^(const )?(boost::safe_numbers::detail::unsigned_integer_basis<unsigned int>|(\w+::)*u32)( &| \*)?$"
    u64_pattern = r"^(const )?(boost::safe_numbers::detail::unsigned_integer_basis<unsigned long>|boost::safe_numbers::detail::unsigned_integer_basis<unsigned long long>|(\w+::)*u64)( &| \*)?$"

    debugger.HandleCommand(
        f'type summary add -x "{u8_pattern}" -e -F safe_numbers_printer.u8_summary'
    )
    debugger.HandleCommand(
        f'type synthetic add -x "{u8_pattern}" -l safe_numbers_printer.UintSyntheticProvider'
    )

    print("u8 printer loaded successfully")

    debugger.HandleCommand(
        f'type summary add -x "{u16_pattern}" -e -F safe_numbers_printer.u16_summary'
    )
    debugger.HandleCommand(
        f'type synthetic add -x "{u16_pattern}" -l safe_numbers_printer.UintSyntheticProvider'
    )

    print("u16 printer loaded successfully")

    debugger.HandleCommand(
        f'type summary add -x "{u32_pattern}" -e -F safe_numbers_printer.u32_summary'
    )
    debugger.HandleCommand(
        f'type synthetic add -x "{u32_pattern}" -l safe_numbers_printer.UintSyntheticProvider'
    )

    print("u32 printer loaded successfully")

    debugger.HandleCommand(
        f'type summary add -x "{u64_pattern}" -e -F safe_numbers_printer.u64_summary'
    )
    debugger.HandleCommand(
        f'type synthetic add -x "{u64_pattern}" -l safe_numbers_printer.UintSyntheticProvider'
    )

    print("u64 printer loaded successfully")


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
