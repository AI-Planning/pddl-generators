#!/usr/bin/env python

import argparse
from itertools import product
import random

import sys


def generate_for_solver(instance_name, num_decks, num_suits, num_values, s):
    lines = []
    deals, piles = s
    lines.append(f"{num_decks} {num_suits} {num_values}")

    lines.append(str(len(deals)))
    for deal in deals:
        lines.append(" ".join([str(len(deal))] + ["%s %s %s" % card for card in deal]))

    lines.append(str(len(piles)))
    for card_stack in piles:
        lines.append(
            " ".join(
                [str(len(card_stack))] + ["%s %s %s" % card for card in card_stack]
            )
        )

    return lines


def generate_readable(instance_name, num_decks, num_suits, num_values, s):
    lines = []
    deals, piles = s
    lines += [
        "Using %s decks of cards with %s suits per deck and %s values per suit"
        % (num_decks, num_suits, num_values)
    ]
    lines += ["", "Deals"]
    deck = [c for deal in deals for c in deal]
    for i, deal in enumerate(deals):
        lines += [
            "deal %s: " % i
            + " ".join(
                [f"d{deck}-s{suite}-v{value}" for deck, suite, value in deal]
            )
        ]
    lines += ["", "Initial configuration of piles"]
    for i, pile in enumerate(piles):
        lines += [
            "pile %s: " % i
            + " ".join(
                [f"d{deck}-s{suite}-v{value}" for deck, suite, value in pile]
            )
        ]
    return lines


def generate_for_pddl(instance_name, num_decks, num_suits, num_values, s):
    deals, piles = s

    comments = generate_readable(instance_name, num_decks, num_suits, num_values, s)

    objects = []
    objects += [
        f"card-d{deck}-s{suit}-v{value} - card"
        for deck in range(num_decks)
        for suit in range(num_suits)
        for value in range(num_values)
    ]
    objects += ["pile-%s - tableau" % pile_id for pile_id in range(len(piles))]
    objects += ["deal-%s - deal" % deal_id for deal_id in range(len(deals) + 1)]

    facts = []
    # Cards in tableau piles
    for pile_id, pile in enumerate(piles):
        facts += [
            "(on card-d%s-s%s-v%s card-d%s-s%s-v%s)" % (card1 + card2)
            for card1, card2 in zip(pile[1:], pile)
        ]
        facts += ["(on card-d%s-s%s-v%s pile-%s)" % (pile[0] + (pile_id,))]
        facts += ["(clear card-d%s-s%s-v%s)" % pile[-1]]
        facts += [f"(part-of-tableau pile-{pile_id} pile-{pile_id})"]
        facts += [
            "(part-of-tableau card-d%s-s%s-v%s pile-%s)" % (card + (pile_id,))
            for card in pile
        ]
        pile_parts = split_pile_in_movable_parts(pile)
        facts += ["(movable card-d%s-s%s-v%s)" % card for card in pile_parts[-1]]
        facts += ["(in-play card-d%s-s%s-v%s)" % card for card in pile]

    # Cards in deals
    for deal_id, deal in enumerate(deals):
        deal_deck = list(reversed(deal))
        facts += ["(on card-d%s-s%s-v%s deal-%s)" % (deal_deck[-1] + (deal_id,))]
        facts += [
            "(on card-d%s-s%s-v%s card-d%s-s%s-v%s)" % (card2 + card1)
            for card1, card2 in zip(deal_deck[1:], deal_deck)
        ][::-1]
        facts += ["(clear card-d%s-s%s-v%s)" % deal_deck[0]]

    facts += ["(current-deal deal-0)"]

    # Static predicates
    facts += [
        "(CAN-CONTINUE-GROUP card-d%s-s%s-v%s card-d%s-s%s-v%s)"
        % (deck1, suit, value, deck2, suit, value + 1)
        for deck1 in range(num_decks)
        for deck2 in range(num_decks)
        for suit in range(num_suits)
        for value in range(num_values - 1)
    ]
    facts += [
        "(CAN-BE-PLACED-ON card-d%s-s%s-v%s card-d%s-s%s-v%s)"
        % (deck1, suit1, value, deck2, suit2, value + 1)
        for deck1 in range(num_decks)
        for deck2 in range(num_decks)
        for suit1 in range(num_suits)
        for suit2 in range(num_suits)
        for value in range(num_values - 1)
    ]
    facts += [
        f"(IS-ACE card-d{deck}-s{suit}-v0)"
        for deck in range(num_decks)
        for suit in range(num_suits)
    ]
    facts += [
        "(IS-KING card-d%s-s%s-v%d)" % (deck, suit, num_values - 1)
        for deck in range(num_decks)
        for suit in range(num_suits)
    ]
    facts += [
        f"(NEXT-DEAL deal-{deal_id} deal-{deal_id + 1})"
        for deal_id in range(len(deals))
    ]
    for deal_id, deal in enumerate(deals):
        deal_deck = list(reversed(deal))
        facts += [
            "(TO-DEAL card-d%s-s%s-v%s pile-%s deal-%s deal-%s)"
            % (deal_deck[-1] + (len(deal) - 1, deal_id, deal_id))
        ]
        facts += [
            "(TO-DEAL card-d%s-s%s-v%s pile-%s deal-%s card-d%s-s%s-v%s)"
            % (nextcard + (pile_id, deal_id) + card)
            for pile_id, (card, nextcard) in enumerate(zip(deal_deck[1:], deal_deck))
        ]

    facts += ["(= (total-cost) 0)"]

    goals = (
        ["(clear pile-%s)" % pile_id for pile_id in range(len(piles))]
        + ["(clear deal-%s)" % deal_id for deal_id in range(len(deals))]
        + [
            f"(on card-d{deck}-s{suit}-v{value} discard)"
            for deck in range(num_decks)
            for suit in range(num_suits)
            for value in range(num_values)
        ]
    )

    lines = []
    lines += ["(define (problem %s)" % instance_name]
    lines += ["(:domain spider)"]
    lines += ["; %s" % c for c in comments]
    lines += ["(:objects"]
    lines += ["    %s" % o for o in objects]
    lines += [")", "(:init"]
    lines += ["    %s" % f for f in facts]
    lines += [")", "(:goal (and"]
    lines += ["    %s" % g for g in goals]
    lines += ["))", "(:metric minimize (total-cost))"]
    lines += [")"]
    return lines


def is_compatible(card1, card2):
    _, suit1, value1 = card1
    _, suit2, value2 = card2
    return suit1 == suit2 and value1 == value2 + 1


def split_pile_in_movable_parts(pile):
    parts = []
    current_part = []
    for card in pile:
        if current_part and not is_compatible(current_part[-1], card):
            parts.append(current_part)
            current_part = []
        current_part.append(card)
    parts.append(current_part)
    return parts


def random_state(num_decks, num_suits, num_values, num_piles, num_deals):
    cards = list(product(range(num_decks), range(num_suits), range(num_values)))
    random.shuffle(cards)
    # use the first cards for the deals
    deals = [cards[i * num_piles : (i + 1) * num_piles] for i in range(num_deals)]
    # distribute remaining cards as evenly as possible
    cards = cards[num_deals * num_piles :]
    num_cards_per_pile = int(len(cards) / num_piles)

    piles = [
        cards[i * num_cards_per_pile : (i + 1) * num_cards_per_pile]
        for i in range(num_piles)
    ]
    # distribute remaining cards to the first piles
    cards = cards[num_piles * num_cards_per_pile :]
    for pile, card in zip(piles, cards):
        pile.append(card)
    return deals, piles


def parse():
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "num_decks", type=int, help="Number of decks of cards used for the game."
    )
    parser.add_argument(
        "num_suits", type=int, help="Number of different suits of cards in each deck."
    )
    parser.add_argument(
        "num_values",
        type=int,
        help="Number of different values of each suit in each deck.",
    )
    parser.add_argument("num_piles", type=int, help="Number of piles.")
    parser.add_argument(
        "num_deals",
        type=int,
        help="Number of deals (each deal contains one card for each pile, remaining cards are distributed to the piles).",
    )
    parser.add_argument("random_seed", type=int)
    parser.add_argument("output_type", choices=["pddl", "raw", "readable"])
    return parser.parse_args()


def main():
    args = parse()
    instance_name = "spider-{}-{}-{}-{}-{}-{}".format(
        args.num_decks,
        args.num_suits,
        args.num_values,
        args.num_piles,
        args.num_deals,
        args.random_seed,
    )

    random.seed(args.random_seed)
    s = random_state(
        args.num_decks, args.num_suits, args.num_values, args.num_piles, args.num_deals
    )
    if args.output_type == "raw":
        lines = generate_for_solver(
            instance_name, args.num_decks, args.num_suits, args.num_values, s
        )
    elif args.output_type == "pddl":
        lines = generate_for_pddl(
            instance_name, args.num_decks, args.num_suits, args.num_values, s
        )
    elif args.output_type == "readable":
        lines = generate_readable(
            instance_name, args.num_decks, args.num_suits, args.num_values, s
        )
    print("\n".join(lines))


if __name__ == "__main__":
    main()
