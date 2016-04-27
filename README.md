# linkprediction

The getSubset function constructs a set reachable from the set\_src using the specified path "i" or "o". "i" means incoming edge and "o" means outgoing edge.

      <---o---
paper1         paper2
       ---o--> 
^  |           |   ^
|  o           o   |
o  |           |   o 
|  V           V   |
author1        author2

here author2 cites author1's paper "paper1" on his paper2. New paper cites old papers. 

getNeighbors does the same but use a format parameter to identify the meta-path. For exmaple, "oPiA" means coauthor from the type "A" node (i.e. author).
# graph_support
