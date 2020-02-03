# MiniAF

## Table of Contents
1. [System Overview](#system-overview)
2. [Input Formats](#input-formats)
3. [Supported Problems and Semantics](#supported-problems-and-semantics)
4. [Solver Interface](#solver-interface)
5. [Build](#build)

## System Overview
**MiniAF** is a SAT-based abstract argumentation solver. It's written in C and based on the **(j)ArgSemSAT**[[1]](#1)[[2]](#2) approach. The general idea is to solve argumentation problems, by finding models of propositional formulas, with iterative calls to a SAT solver. 
In principle, this task can be broken down into three subtasks: 1) Transforming the argumentation problem to a SAT problem; 2) Communication with an external SAT solver; and 3) Meaningful interpretation of the SAT solver output, with regard to the original argumentation problem.   

Following this approach,  to solve any computational problem for a given semantics and abstract argumentation framework (AF), a propositional formula-which correspondes to a complete labelling-has to be identified. 
Analogous to the three possible labels *in*, *out* and *undec*, each argument is represented by three boolean variables.  The resulting formula is in conjunctive normal form (CNF), as from SAT solvers required.

To compute e.g. a complete extension this formula is passed to an external SAT solver, which checks if there is an assignment of the boolean variables, so that the formula evaluates to *True*. If there is no such assignment, there is no complete extension. However, if there is an assignment, each variable is assigned a label (in, out, undec) depending on its truth value. The set of variables declared as *in*, form a complete extension. In order to enumerate all complete extensions, the previous found model respectively labelling has to be excluded from the formula. This is achieved by appending the negation of the previous found labelling. The modified formula is then passed back to the SAT solver. This procedure is repeated until the formula is no longer satisfiable, i.e. no further labelling can be found. In this way, all extensions are enumerated.

The computation of stable extensions or rather stable labellings works basicly the same way. ...

## Input Formats

The solver supports two different file formats:

* **Trivial Graph Format (.tgf)**
* **Aspartix Format (.apx)**


**Examples**
* Trivial Graph Format (.tgf)
```
1 (First node)
2 (Second node)
#
1 2 (Edge between the two)
```

* Aspartix Format (.apx)
```
arg(a1).
arg(a2).
arg(a3).
att(a1,a2).
att(a2,a3).
att(a2,a1).

```

Additional information about the Trivial Graph Format can be found here:  http://en.wikipedia.org/wiki/Trivial_Graph_Format

For further information on the Aspartic Format see https://www.iccma2019.dmi.unipg.it/res/SolverRequirements.pdf

## Supported Problems and Semantics

The following semantics are supported:

* **Complete Semantics (CO)**
* **Stable Semantics (ST)**
* **Preferred Semantics (PR)**
* **Grounded Semantics (GR)**

For every semantics the solver solves four different problems:

* **Enumerate all extensions (EE)**
* **Enumerate some extension (SE)**
* **Credulously accepted (DC)**
* **Skeptical accepted (DS)**

## Solver Interface

The solver is runnable from a command line and provides all behaviors described here: https://www.iccma2019.dmi.unipg.it/res/SolverRequirements.pdf (Chapter 7)

In general, the solver can be parameterized in the following way:
* **solver -p \<task\> -f \<file\> -fo \<fileformat\> \[-a \<additional parameter\>\] -sat \<satpath\> \[-satparam \<additional parameter sat\>\]**
 
**-p**

The computational problem can be specified with the parameter *task* in the following way: **Problem-Semantics**

Example: **DC-CO** 

**-f**

The parameter *file* is the absolute path to the input file.

**-fo**

*\<fileformat\>* must be either **apx** or **tgf**. This parameter must match the file extension of the test file.

**-a**

For the **DC** and **DS** problems an additional parameter must be specified.The *\<additional parameter\>* has to be an argument contained in the testfile *file*.  


 **-sat**
 
The solver can be parameterized with any SAT solver that supports the simplified version of the DIMACS format( see http://www.satcompetition.org/2009/format-benchmarks2009.html) and expects its input through the standard input stream. Also the solver has to  writes its result to the standard output stream.  stream.

*\<satpath\>*  must be the absolute path to the executable file of the SAT solver.

**-satparam**

In addition, the SAT solver can also be parameterized. The specific parameters of each SAT solver can be passed in argument *\<additional parameter sat\>*.

**Note**: Some SAT solvers have to be executed with a specific parameter such as *-model*, as they do not write the resulting model to the console by default.

**Example**
```
**TODO**
```


* **solver --formats**

Prints list of supported formats to the console

```
$user MiniAF --formats
```

* **solver --problems**

Prints list of supported computational problems to the console

```
$user MiniAF --problems
```


## Build
Just run the *build.sh* script.


## References
<a id="1">[1]</a> 
Federico Cerutti, Massimiliano Giacomin, and Mauro Vallati.
ArgSemSAT: solving argumentation problems using SAT. In Simon Parsons, Nir
Oren, Chris Reed, and Federico Cerutti, editors, Proceedings of the 5th International
Conference on Computational Models of Argument (COMMA 2014), volume 266 of
Frontiers in Artificial Intelligence and Applications, pages 455–456. IOS Press, 2014.

<a id="2">[2]</a> 
Federico Cerutti, Mauro Vallati, and Massimiliano Giacomin.
jArgSemSAT: an efficient off-the-shelf solver for abstract argumentation frameworks.
In James P. Delgrande and Frank Wolter, editors, 15th International Conference on
Principles of Knowledge Representation and Reasoning (KR2016), pages 541–544,
2016.










