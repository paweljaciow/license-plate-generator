/**
* This file is part of the License Plate Generator project
* modified:    2017-07-23
* author:      Paweł Jaciów
* e-mail:      pawel.jaciow@gmail.com
*/

This file explains how the LicensePlates are ordered.

Ordering assumes that LicensePlatePatterns are defined with symbols sorted by their costs, lowest cost first. Cost is opposite of probability - the lowest cost, the higher probability.

Given this assumption it is straightforward that the most probable combination is that of the first symbol from each pattern. Cost of a combination is defined as the sum of its symbol costs. This combination is pushed first into sorted list of combinations.

Next most probable combination would be different from the best one by one symbol shift. This means combination of the first symbol from each pattern but one and the second symbol from that one pattern. At this point it is not clear which pattern that would be. So we have to compute cost of all combinations differrent from best one by one symbol shift. Such generated combinations we sort by their cost (lowest cost firts) which gives us a combination queue.

First combination from the queue is considered the best. It is removed from the queue and pushed to sorted list of combinations. Then, analogically all combinations different by one symbol shift from that one removed are generated and sorted in the queue with combinations generated in previous steps.

This procedure is repeated until all LicensePlates are generated in an ordered list.

At every step it is required to check uniqeness of combination - no duplicates are allowed.


Example run:
pattern-1: A,B,C | cost-1: 0,1,2
pattern-2: 1,2 | cost-2: 0,1

Best combination (first symbol from each pattern): A1 (cost 0+0=0)
Ordered combinations: [A1]
Combinations different by one symbol shift from A1: B1 (cost 1+0=1), A2 (cost 0+1=1)
Queue: [B1, A2]

Best combination: B1
Ordered combinations: [A1, B1]
Combinations different by one symbol shift from B1: C1 (cost 2+0=2), B2 (cost 1+1=2)
Queue: [A2, C1, B2]

Best combination: A2
Ordered combinations: [A1, B1, A2]
Combinations different by one symbol shift from A2: B2 - discarded (duplicate)
Queue: [C1, B2]

Best combination: C1
Ordered combinations: [A1, B1, A2, C1]
Combinations different by one symbol shift from C1: C2 (cost 2+1=3)
Queue: [B2, C2]

Best combination: B2
Ordered combinations: [A1, B1, A2, C1, B2]
Combinations different by one symbol shift from B2: C2 - discarded (duplicate)
Queue: [C2]

Best combination: C2
Ordered combinations: [A1, B1, A2, C1, B2, C2]
Combinations different by one symbol shift from B1: none - finished
Queue: []
