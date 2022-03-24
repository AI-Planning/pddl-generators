## Freecell

* Origin: Fahiem Bacchus. Used in the AIPS-2000 competition.
* Adaptions: Domain encoding modified such that cards, freecells, and columns
  all have their own natural numbers. This helps avoiding superfluous action
  instances.
* Description: Typed STRIPS encoding of a card game (similar to Solitaire)
  that comes free with Microsoft Windows. Given a random configuration of cards
  across some columns, move all cards in a specified order onto some goal
  stacks, obeying a number of stacking rules, and using a number of freecells as
  a resource.
* Parameters:
  * -f number of freecells (minimal 0)
  * -c number of columns (minimal 1)
  * -s number of suits (minimal 1, maximal 4)
  * -i number of initial stacks (minimal 1)
  * -0 .. -3 number of cards in each suite
  * -r random seed (optional)
* Generation: As long as there is a card that has not yet been placed somewhere,
  choose one such card at random, and place it randomly on one initial stack.

IPC Instances

In the IPC there were two separate sets. All instances have 4 suits.

SET1:
4 suits, 3 to 13 cards per suite, from 4 to 8 columns

SET2: Scales cards from 2 to 13, 4 instances per size.
