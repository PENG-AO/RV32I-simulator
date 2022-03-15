# risc-v simulator

## 1. Update log

- [2022/02/25] simplify simulator/assembler to RV32I

## 2. How to use

***recommended workflow as well***

### 2.1 Prerequisites

- gcc (for simulator)
- ncurses (for simulator)

	> MacOS: supported
	> 
	> Ubuntu: run `sudo apt-get install libncurses5-dev`

	> simulator looks better in brighter colors and here is a setting I personally prefer
	> 
	> | color   | hex code | sample                                                                                 |
	> | :------ | :------- | :------------------------------------------------------------------------------------- |
	> | black   | #1C2126  | <img src='https://www.thecolorapi.com/id?format=svg&named=false&hex=1C2126' height=48> |
	> | red     | #E06C75  | <img src='https://www.thecolorapi.com/id?format=svg&named=false&hex=E06C75' height=48> |
	> | green   | #98C379  | <img src='https://www.thecolorapi.com/id?format=svg&named=false&hex=98C379' height=48> |
	> | yellow  | #E5C07B  | <img src='https://www.thecolorapi.com/id?format=svg&named=false&hex=E5C07B' height=48> |
	> | blue    | #61AFEF  | <img src='https://www.thecolorapi.com/id?format=svg&named=false&hex=61AFEF' height=48> |
	> | magenta | #C678DD  | <img src='https://www.thecolorapi.com/id?format=svg&named=false&hex=C678DD' height=48> |
	> | cyan    | #56B6C2  | <img src='https://www.thecolorapi.com/id?format=svg&named=false&hex=56B6C2' height=48> |
	> | white   | #DCDFE4  | <img src='https://www.thecolorapi.com/id?format=svg&named=false&hex=DCDFE4' height=48> |

- python3.8 (for assembler)

### 2.2 Assembler

- step1: `cd ./asm`
- step2: check `python3 asm.py -h` for help
- step3: do some executions like `python3 asm.py test-codes/fib.s`
- step4: check outputs in `./bin`

	> `xxd fileName.code` or `hexdump fileName.code` would be helpful

### 2.3 Simulator

> customizable settings
> 
> - Cache (under `src/cache.h`)
> 	- block size
> 	- associativity
>	- block switching policy (specially optimized for LRU)
> - Branch Predictor (under `src/branch_predictor.h`)
> 	- prediction policy
> 	- size of PHT
> 
> run `make clean; make` to apply changes

- step1: `make sim`
- step2: do some executions like `./sim bin/fib.bin`
- step3: type `help` to get more information about gui mode
- step4: type `quit` to exit simulator
- step5: `make clean`
