""" File:        generator_cmd_line.py
    Author:      Nathan Robinson
    Contact:     nathan.m.robinson@gmail.com
    Date:        2013-11-13

    Desctiption: This file uses cmd_line.py to make a command line interface
                 for the cave diving PDDL problem generator.

    Lisence:    Copyright (c) Year 2013, Nathan Robinson <nathan.m.robinson@gmail.com>
                                         Christian Muise <christian.muise@gmail.com>
                                         Charles Gretton <charles.gretton@gmail.com>

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

from cmd_line import (
    ArgProcessor,
    ArgDefinition,
    FlagDefinition,
    range_validator,
    seq_range_validator,
    bool_validator,
    InputException,
)


def print_citation_info(arg_processor):
    print("When citing this program please cite the following publication:")
    print("TBA")
    sys.exit(0)


def process_args():
    arg_processor = ArgProcessor()

    arg_processor.add_program_arg(
        "-domain_file",
        ArgDefinition(
            "domain_file_name", False, None, None, None, "The output PDDL domain file"
        ),
    )

    arg_processor.add_program_arg(
        "-problem_file",
        ArgDefinition(
            "problem_file_name", True, None, None, None, "The output PDDL problem file"
        ),
    )

    arg_processor.add_program_arg(
        "-problem_name",
        ArgDefinition(
            "problem_name", True, None, None, None, "The name of the problem instance"
        ),
    )

    arg_processor.add_program_arg(
        "-cave_branches",
        ArgDefinition(
            "cave_branches",
            False,
            seq_range_validator,
            [":", 1, None, int, 2, None, "Error - invalid cave branches:"],
            [3],
            "Depths of the cave branches (x0:...:xk)",
        ),
    )

    arg_processor.add_program_arg(
        "-objectives",
        ArgDefinition(
            "objectives",
            False,
            seq_range_validator,
            [":", 1, None, int, 1, None, "Error - photo objectives:"],
            [3],
            "The depths of the photo objectives (x0:...:xk)",
        ),
    )

    arg_processor.add_program_arg(
        "-num_tank_adjustment",
        ArgDefinition(
            "num_tank_adjustment",
            False,
            range_validator,
            [int, None, None, False, "Error - invalid number of tanks adjustment:"],
            0,
            "How many more/fewer tanks than required to use",
        ),
    )

    arg_processor.add_program_arg(
        "-num_diver_adjustment",
        ArgDefinition(
            "num_diver_adjustment",
            False,
            range_validator,
            [int, None, None, False, "Error - invalid number of divers adjustment:"],
            0,
            "How many more/fewer divers than required to use",
        ),
    )

    arg_processor.add_program_arg(
        "-neg_link_prob",
        ArgDefinition(
            "neg_link_prob",
            False,
            range_validator,
            [float, 0, 1, False, "Error - invalid negative link probability:"],
            0.5,
            "The probability of a negative diver relationship link",
        ),
    )

    arg_processor.add_program_arg(
        "-neg_cycle_length",
        ArgDefinition(
            "neg_cycle_length",
            False,
            range_validator,
            [float, 0, 1, True, "Error - invalid negative cycle length:"],
            None,
            "The frac of divers in a negative cycle (None=no cycle)",
        ),
    )

    arg_processor.add_program_arg(
        "-minimum_hiring_cost",
        ArgDefinition(
            "minimum_hiring_cost",
            False,
            range_validator,
            [int, 0, None, False, "Error - invalid minimum hiring cost:"],
            10,
            "The minimum cost of any hiring action",
        ),
    )

    arg_processor.add_program_arg(
        "-maximum_hiring_cost",
        ArgDefinition(
            "maximum_hiring_cost",
            False,
            range_validator,
            [int, 0, None, False, "Error - invalid maximum hiring cost "],
            100,
            "The maximum cost of any hiring action",
        ),
    )

    arg_processor.add_program_arg(
        "-perturb_hiring_costs",
        ArgDefinition(
            "perturb_hiring_costs",
            False,
            range_validator,
            [float, 0, 1, False, "Error - invalid perturb hiring cost fraction:"],
            0,
            "How much hiring costs should be perturbed",
        ),
    )

    arg_processor.add_program_arg(
        "-other_action_cost",
        ArgDefinition(
            "other_action_cost",
            False,
            range_validator,
            [float, 0, None, False, "Error - invalid other hiring cost:"],
            1,
            "The cost of non-hiring actions",
        ),
    )

    arg_processor.add_program_arg(
        "-strips",
        ArgDefinition(
            "strips",
            False,
            bool_validator,
            ["Error - invalid STRIPS setting:"],
            False,
            "If the domain should be compiled to STRIPS",
        ),
    )

    arg_processor.add_program_arg(
        "-order_tanks",
        ArgDefinition(
            "order_tanks",
            False,
            bool_validator,
            ["Error - invalid order tanks setting:"],
            True,
            "Force an order of the preparation of the tanks",
        ),
    )

    arg_processor.add_program_arg(
        "-seed",
        ArgDefinition(
            "seed",
            False,
            range_validator,
            [int, None, None, True, "Error - invalid seed: "],
            None,
            "Random seed (None for system time)",
        ),
    )

    arg_processor.add_program_flag(
        "--cite",
        FlagDefinition("cite", print_citation_info, "Display citation information"),
    )

    try:
        arg_processor.parse_args()

        if len(arg_processor.objectives) > len(arg_processor.cave_branches):
            raise InputException(
                "Not enough cave branches for number of photo targets."
            )

        if arg_processor.maximum_hiring_cost < arg_processor.minimum_hiring_cost:
            raise InputException("Max hiring cost is less than max.")

    except InputException as e:
        print(e.message)
        print("Use --help flag to display usage information.")
        sys.exit(1)
    return arg_processor
