# Classical Engine for simple games.

import random

class TTT() :

    def __init__(self):
        self.grid= {
            l: [0 for i in range(4) ]
            for l in ["A", "B", "C"]
        }

    def name(self):
        return "TicTacToe Standard"

    def status(self):
        s= f"A-1: {self.grid['A'][1]}\nA-2: {self.grid['A'][2]}\nA-3: {self.grid['A'][3]}"
        for l in ["B", "C"]:
            for i in range(1,4) :
                s+= f"\n{l}-{i}: {self.grid[l][i]}"
        return s
    
    def isEnded(self) :
        return self.isWinning(1) or self.isWinning(2) or self.count(0) == 0
    
    def isWinning(self, playerId) :
        win= False
        for abs in ["A", "B", "C"]:
            win= win or ( self.grid[abs][1] == playerId and self.grid[abs][2] == playerId and self.grid[abs][3] == playerId )
        for ord in [1, 2, 3]:
            win= win or ( self.grid['A'][ord] == playerId and self.grid['B'][ord] == playerId and self.grid['C'][ord] == playerId )
        win= win or ( self.grid['A'][1] == playerId and self.grid['B'][2] == playerId and self.grid['C'][3] == playerId )
        win= win or ( self.grid['A'][3] == playerId and self.grid['B'][2] == playerId and self.grid['C'][1] == playerId )
        return win

    def count(self, playerId) :
        c= 0
        for abs in ["A", "B", "C"]:
            for ord in [1, 2, 3]:
                if self.grid[abs][ord] == playerId :
                    c+= 1
        return c

    def apply(self, playerId, position):
        abs, ord= tuple( position.split('-') )
        ord= int(ord)
        if abs in ["A", "B", "C"] and 0 < ord and ord <= 3  and self.grid[abs][ord] == 0 :
            self.grid[abs][ord] = playerId
            return True
        return False

    def __str__(self) :
        sign= [' ', 'x', 'o', '-']
        abss= ['A', 'B', 'C']
        s= '  '
        for abs in abss :
            s+= ' '+ abs
        for ord in range(1, 4) :
            s+= '\n'+ str(ord) +' '
            for abs in abss :
                s+= ' '+ sign[ self.grid[abs][ord] ]
        return s

class Ultimate() :

    def __init__(self):
        self.grid= {
            l: [0 for i in range(10) ]
            for l in ["A", "B", "C", "D", "E", "F", "G", "H", "I"]
        }
        self.targets= [1, 2, 5]

    def name(self):
        return "TicTacToe Ultimate"
    
    def subGrid(self, i):
        if i in [1, 4, 7] :
            abss= [ "A", "B", "C" ]
        elif i in [2, 5, 8] :
            abss= [ "D", "E", "F" ]
        else :
            abss= [ "G", "H", "I" ]
    
        if i in [1, 2, 3] :
            ords= range(1, 4)
        elif i in [4, 5, 6] :
            ords= range(4, 7)
        else :
            ords= range(7, 10)
            
        return abss, ords
    
    def status(self):
        s= f"A-1: {self.grid['A'][1]}"
        for i in range(2, 10) :
            s+= f"\nA-{i}: {self.grid['A'][i]}"
        for l in ["B", "C", "D", "E", "F", "G", "H", "I"]:
            for i in range(1, 10) :
                s+= f"\n{l}-{i}: {self.grid[l][i]}"
        s+= "\ntargets: " + ' '.join( [str(x) for x in self.targets] )
        return s
    
    def apply(self, playerId, position):
        if self.isValidAction( position ) :
            abs, ord= tuple( position.split('-') )
            ord= int(ord)
            self.grid[abs][ord] = playerId
            self.targets= self.actionTargets( abs, ord )
            return True
        return False
    
    def isValidAction(self, a):
        actions= []
        for i in self.targets :
            abss, ords= self.subGrid( i )
            for l in abss :
                actions+= [ f"{l}-{i}" for i in ords ]
        return (a in actions)

    def actionTargets( self, a, o ):
        # Compute the default target:
        a= ( (ord(a)-ord('A')) % 3 )
        o= (o-1)%3
        target= o*3+a+1
        # Test if the target is ok ?
        if self.isTargetOk( target ) :
            return [ target ]
        # else add all the other target...
        targets= []
        for iTarget in range(1, 10) :
            if iTarget != target and self.isTargetOk( iTarget ) :
                targets.append(iTarget)
        return targets

    def subTTT(self, target) :
        ttt= TTT()
        abss, ords= self.subGrid(target)
        for a1, a2 in zip(['A', 'B', 'C'], abss)  :
            for o1, o2 in zip([1,2,3], ords) :
                ttt.grid[a1][o1]= self.grid[a2][o2]
        return ttt
    
    def isTargetOk(self, target) :
        ttt= self.subTTT(target)
        return not ttt.isEnded()

    def superTTT(self) :
        match= ["",
            "A-1", "B-1", "C-1",
            "A-2", "B-2", "C-2",
            "A-3", "B-3", "C-3",
        ]

        superTTT= TTT()
        for iTTT in range(1, 10) :
            ttt= self.subTTT( iTTT )
            if ttt.isWinning(1) :
                superTTT.apply( 1, match[iTTT] )
            elif ttt.isWinning(2) :
                superTTT.apply( 2, match[iTTT] )
            elif ttt.count(0) == 0 :
                superTTT.apply( 3, match[iTTT] )
        return superTTT
    
    def isEnded(self) :
        # Is end ?
        superTTT= self.superTTT()
        return superTTT.isEnded()

    def isWinning(self, playerId) :
        superTTT= self.superTTT()
        return superTTT.isWinning(playerId)