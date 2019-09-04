""" File:        cmd_line.py
    Author:      Nathan Robinson
    Contact:     nathan.m.robinson@gmail.com
    Date:        2012-07-07
    Desctiption: This module describes a generic command line input processor
                 for python programs

    Liscence:   Copyright (c) Year 2012, Nathan Robinson <nathan.m.robinson@gmail.com>

                Permission to use, copy, modify, and/or distribute this software for any
                purpose with or without fee is hereby granted, provided that the above
                copyright notice and this permission notice appear in all copies.

                THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
                WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
                MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
                SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
                WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
                ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR
                IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
"""

import sys

# Preliminary definitions
# -------------------------------------------------------------------------------


class InputException(Exception):
    """A custom Exception thrown by ArgProcessor.parse_args(), et al."""


class ArgDefinition:
    """This class stores the definition of a command line argument that can be
        passed to the program.
    """

    def __init__(
        self, var_name, needed, validator, validator_args, default_value, description
    ):
        """ Create a new ArgDefinition.

            (ArgDefinition, str, bool, function, [object], object, str) -> None
            var_name: The name of the variable where the value will be stored.
            needed: If the argument can be is required (default not used then).
            validator: A function pointer that will take the read in and
                validate the value of this argument and either throw an
                InputException or return the parsed value (None for no
                validation).
            validator_args: A list of the arguments (except the value) for the
                validator function.
            default_value: The value that the var_name will be set to if one is
                not given on the commane line. This will not be validated.
            description: A string describing the argument to show after parsing
                if the the verbose option is selected (it is by default).
        """

        self.var_name = var_name
        self.needed = needed
        self.validator = validator
        self.validator_args = validator_args
        self.default_value = default_value
        self.description = description


class FlagDefinition:
    """ This class stores the definition of a command line flag that can be
        passed to the program.
    """

    def __init__(self, var_name, function, description):
        """ Create a new FlagDefinition.

            (FlagDefinition, str, function, str) -> None
            var_name: The name of the variable where the value will be stored.
            function: A pointer to a function that will be called if the flag is
                present (or None). Flag functions are called in the order that
                flags are defined in, before arguments are passed.
                The function must take a single argument, the ArgProcessor.
            description: A string describing the flag to show after parsing
                unless the suppress flag is supplied.
        """

        self.var_name = var_name
        self.function = function
        self.description = description


# Validators
# A set of functions that validate the various types of command line arguments.
# -------------------------------------------------------------------------------


def range_validator(value_str, args):
    """ Validate command line args that take a range of values.

        (str, [type, object, object, bool, str]) -> object

        value_str: The command line string representing the argument.
        args: A list containing the parameters that the validator requires:
              0 - type:       The python type of the cmd line arg;
              1 - lb:         The lower bound of the cmd line arg (or None);
              2 - ub:         The upper bound of the cmd line arg (or None);
              3 - allow_none: If the arg can have the value 'None'; and
              4 - error_msg:  The error message to be printed (followed by the
                              value) if the supplied value is invalid.
    """

    assert len(args) == 5, "Error: range_validator requires 5 arguments."
    a_type, lb, ub, allow_none, error_msg = args
    try:
        if allow_none and value_str == "None":
            value = None
        else:
            value = a_type(value_str)
    except ValueError:
        raise InputException(error_msg + " " + value_str)
    if (lb is not None and value < lb) or (ub is not None and value > ub):
        raise InputException(error_msg + " " + value_str)
    return value


def range_validator_advice(validator_args):
    """ A function for the range_validator that returns a short help string
        based on the supplied args.

        ([type, object, object, bool, str]) -> str
    """

    a_type, lb, ub, allow_none, error_msg = validator_args
    if lb is None and ub is None:
        return ""
    adv_str = "x"
    if lb is not None:
        adv_str = str(lb) + " <= " + adv_str
    if ub is not None:
        adv_str += " <= " + str(ub)
    if allow_none:
        adv_str += ", None"
    return " {" + adv_str + "}"


def enum_validator(value_str, args):
    """ Validate cmd line args that take one of a set of str values.

        (str, [[str], str]) -> str
        value_str: The command line string representing the argument.
        args:      A list containing the parameters that the validator requires:
            0 - v_list: The list of valid string values that the cmd line arg
                        can take; and
            1 - error_msg: The error message to be printed (followed by the
                           value) if the supplied value is invalid.
    """

    assert len(args) == 2, "Error: enum_validator requires 2 arguments."
    v_list, error_message = args
    if v_list is not None and value_str not in v_list:
        raise InputException(error_message + " " + value_str)
    return value_str


def enum_validator_advice(validator_args):
    """ A function for the enum_validator that returns a short help string based
        on the supplied args.

        ([[str], str]) -> str
    """

    return " {" + ", ".join(validator_args[0]) + "}"


def bool_validator(value_str, args):
    """ Validate Boolean command line args.

        (str, [str]) -> bool
        value_str: The command line string representing the arg.
        args:      A list containing the parameters that the validator requires:
                   0 - error_msg: The error message to be printed (followed by
                                  the value) if the supplied value is invalid.
    """

    assert len(args) == 1, "Error: bool_validator requires 1 argument."
    error_message = args[0]
    if value_str == "True":
        value = True
    elif value_str == "False":
        value = False
    else:
        raise InputException(error_message + " " + value_str)
    return value


def bool_validator_advice(validator_args):
    """ A function for the bool_validator that returns a short help string

        ([str]) -> str
    """

    return " {True, False}"


def seq_range_validator(value_str, args):
    """ Validate a sequence of command line args that take a range of values,
        each value is seperated by the specified seperator.

        (str, [str, type, object, object, bool, str]) -> [Number]

        value_str: The command line string representing the argument.
        args: A list containing the parameters that the validator requires:
              0 - sep:        The seperator used to separate the list of values
              1 - min_vals:   The minimum number of values (or None);
              2 - max_vals:   The maximum number of values (or None);
              3 - type:       The python type of the cmd line arg;
              4 - lb:         The lower bound of the values (or None);
              5 - ub:         The upper bound of the values (or None);
              6 - error_msg:  The error message to be printed (followed by the
                              value) if the supplied value is invalid.
    """

    assert len(args) == 7, "Error: seq_range_validator requires 7 arguments."
    sep, min_vals, max_vals, a_type, lb, ub, error_msg = args
    try:
        values = value_str.split(sep)
        if (min_vals is not None and len(values) < min_vals) or (
            max_vals is not None and len(values) > max_vals
        ):
            raise InputException(error_msg + " " + value_str)
        values_out = []
        for value in values:
            value = a_type(value)
            if (lb is not None and value < lb) or (ub is not None and value > ub):
                raise InputException(error_msg + " " + value_str)
            values_out.append(value)
        return values_out
    except ValueError:
        raise InputException(error_msg + " " + value_str)


def seq_range_validator_advice(validator_args):
    """ A function for the seq_range_validator that returns a short help string
        based on the supplied args.

        ([type, object, object, bool, str]) -> str
    """

    sep, min_vals, max_vals, a_type, lb, ub, error_msg = validator_args
    k_str = ""
    if min_vals is not None or max_vals is not None:
        k_str = "k"
    if min_vals is not None:
        k_str = str(min_vals) + " <= " + k_str
    if max_vals is not None:
        k_str = k_str + " <= " + str(max_vals)
    x_str = ""
    if lb is not None or ub is not None:
        x_str = "xi"
    if lb is not None:
        x_str = str(lb) + " <= " + x_str
    if ub is not None:
        x_str = x_str + " <= " + str(ub)
    adv_str = ""
    if k_str:
        adv_str += k_str
    if x_str:
        if k_str:
            adv_str += ", "
        adv_str += x_str
    return " {" + adv_str + "}"


def print_usage(arg_processor):
    """ This function prints a usage message to sys.stdout based on the
        current argument and flag definition. It then exits.

        (ArgProcessor) -> None
    """
    min_width = max(
        [
            len(x)
            for x in arg_processor.program_arg_order + arg_processor.program_flag_order
        ]
    )
    print("Usage: python {} ".format(sys.argv[0]))
    print("The following flags and arguments can be supplied:")
    print("Flags:")
    for flag in arg_processor.program_flag_order:
        print(
            "  {:<{}} : {}".format(
                flag, min_width, arg_processor.program_flags[flag].description
            )
        )
    print("Arguments:")
    for arg in arg_processor.program_arg_order:
        if arg_processor.program_args[arg].validator is not None:
            advice_str = arg_processor.advice_functions[
                arg_processor.program_args[arg].validator
            ](arg_processor.program_args[arg].validator_args)
        else:
            advice_str = ""
        if arg_processor.program_args[arg].needed:
            print(
                "  {:<{}} : {}{}".format(
                    arg,
                    min_width,
                    arg_processor.program_args[arg].description,
                    advice_str,
                )
            )
        else:
            print(
                "  {:<{}} : {}{} [optional, default: {}]".format(
                    arg,
                    min_width,
                    arg_processor.program_args[arg].description,
                    advice_str,
                    arg_processor.program_args[arg].default_value,
                )
            )
    sys.exit(0)


# The main class
# -------------------------------------------------------------------------------


class ArgProcessor:
    """ Objects instantiating this class will have a number of argument and
        flag definitions added. Once parse_args() has been called, the
        appropriate local variables of the ArgProcessor object will be set or
        an InputException will be raised.
    """

    def __init__(self):
        """ Make a new ArgProcessor.

            (ArgProcessor) -> None
        """
        self.program_args = {}
        self.program_arg_order = []
        self.program_flags = {}
        self.program_flag_order = []
        self.validators = []
        self.advice_functions = {}
        # Register the pre-made validators
        self.register_validator(range_validator, range_validator_advice)
        self.register_validator(seq_range_validator, seq_range_validator_advice)
        self.register_validator(enum_validator, enum_validator_advice)
        self.register_validator(bool_validator, bool_validator_advice)
        # Add the quiet flag:
        self.add_program_flag(
            "--quiet", FlagDefinition("quiet", None, "Suppress non-essential output")
        )
        # Add the help flag
        self.add_program_flag(
            "--help", FlagDefinition("help", print_usage, "Display this notice")
        )

    def add_program_arg(self, param, arg_definition):
        """ This method adds a program argument.

            (ArgProcessor, str, ArgDefinition) -> None
            param:          The '-' prefixed param string of the argument.
            arg_definition: The ArgDefinition object describing the argument.
        """
        assert param not in self.program_args, "Error: parameter name in use."
        assert (
            arg_definition.validator is None
            or arg_definition.validator in self.validators
        ), "Error: unregistered validator"
        self.program_args[param] = arg_definition
        self.program_arg_order.append(param)

    # This method adds a program flag.
    # param:          The '--' prefixed flag string.
    # arg_definition: The FlagDefinition object describing the flag.
    def add_program_flag(self, flag, flag_definition):
        assert flag not in self.program_flags, "Error: flag name in use."
        self.program_flags[flag] = flag_definition
        self.program_flag_order.append(flag)
        vars(self)[flag_definition.var_name] = False

    def register_validator(self, validator, validator_advice):
        """ This method adds a validator and maps it to the relevant advice
            function.

            (ArgProcessor, function, function) -> None
        """
        self.validators.append(validator)
        self.advice_functions[validator] = validator_advice

    def parse_args(self):
        """ Call this function to parse the input arguments.

            (ArgProcessor) -> None
        """
        # -----------------------------------------------------------------------
        # This code is based on code from the KR Toolkit by Christian Muise
        # URL: http://code.google.com/p/krtoolkit/
        try:
            argv, opts, flags = sys.argv[1:], {}, []
            while argv:
                if argv[0][0:2] == "--":
                    flags.append(argv[0])
                    argv = argv[1:]
                elif argv[0][0] == "-":
                    opts[argv[0]] = argv[1]
                    argv = argv[2:]
                else:
                    raise InputException("Badly constructed arg: " + argv[0])
        except IndexError:
            raise InputException("Badly constructed arg: " + argv[0])
        # -----------------------------------------------------------------------
        for flag in flags:
            if flag in self.program_flags:
                vars(self)[self.program_flags[flag].var_name] = True
                if self.program_flags[flag].function:
                    self.program_flags[flag].function(self)
            else:
                raise InputException("Invalid flag: " + flag)

        if not self.quiet:
            min_width = (
                max(
                    len("Flags:"),
                    max([len(x.description) for x in iter(self.program_args.values())]),
                )
                + 1
            )
            if len(flags) == 0:
                print("{:<{}} {}".format("Flags:", min_width, "<None>"))
            else:
                print(
                    "{:<{}} {}".format(
                        "Flags:",
                        min_width,
                        ", ".join([f for f in self.program_flags if f in flags]),
                    )
                )

        for arg in opts:
            if arg not in self.program_args:
                raise InputException("Invalid arg: " + arg)

        for arg in self.program_arg_order:
            arg_def = self.program_args[arg]
            if arg not in opts:
                if arg_def.needed:
                    raise InputException("Error needed arg is missing: " + arg)
                vars(self)[arg_def.var_name] = arg_def.default_value
            else:
                if arg_def.validator is None:
                    vars(self)[arg_def.var_name] = opts[arg]
                else:
                    vars(self)[arg_def.var_name] = arg_def.validator(
                        opts[arg], arg_def.validator_args
                    )
            if not self.quiet:
                print(
                    "{:<{}} {}".format(
                        arg_def.description + ":",
                        min_width,
                        vars(self)[arg_def.var_name],
                    )
                )
