from pieceOfData import Pod

class Board(Pod):

    def __init__( self, numberOfCells= 0 ):
        super().__init__("Board")
        for i in range(numberOfCells) :
            self._children.append( Pod( f"Cell-{i+1}" ) )
            self._children.append( Pod( f"Edge-{i+1}" ) )
        self.ite= 1

    def numberOfCells(self):
        return len(self._children)//2

    def cell(self, iCell):
        return self._children[ (iCell-1)*2 ]

    def cells(self):
        cells= []
        for cell, edges in self :
            cells.append( cell )
        return cells

    def edges(self, iCell):
        return self._children[ (iCell-1)*2 + 1 ].attributes()

    def isEdge(self, iFrom, iTo):
        return iTo in self.edges(iFrom)

    def connect(self, iFrom, iTo):
        fromEdge= self.edges(iFrom)
        if iTo not in fromEdge :
            fromEdge.append(iTo)
            fromEdge.sort()
        return self

    # Iterator over board cells
    def __iter__(self):
        self.ite = 1
        return self

    def __next__(self):
        if self.ite <= self.numberOfCells() :
            cell = self.cell( self.ite )
            edges= self.edges( self.ite )
            self.ite += 1
            return cell, edges
        else:
            raise StopIteration

    def iCell(self):
        return self.ite-1