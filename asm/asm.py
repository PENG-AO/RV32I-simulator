# assembler

import os, argparse
from decoder import *

class ASM(object):

    DEFAULT_PC = 0x0
    DATA_SECTION = 0
    CODE_SECTION = 1

    def __init__(self) -> None:
        self.fileName = None
        self.rawText = list()
        self.code = list()
        self.codeTag = dict()
        self.data = list()
        self.dataTag = dict()
        self.machineCode = list()

        self.startTag = None
        self.section = ASM.CODE_SECTION
        self.codeCounter = ASM.DEFAULT_PC
        self.dataCounter = 0
    
    def load(self, files: list) -> None:
        self.fileName = files[0].split('/')[-1].split('.')[0]
        for idx, file in enumerate(files, start=1):
            try:
                codeText = open(file)
                # remove all unnecessary parts like space\n\t and comments
                self.rawText.extend([line.lstrip().rstrip().split('#')[0] for line in codeText])
                # print progress
                printProgress(idx, len(files), info='Loading')
            except FileNotFoundError:
                raise RuntimeError(f'invalid file name: {file}.')

    def decode(self) -> None:
        # 3 prepend pseudo instr * 2 assumed length * 4 bytes each
        self.codeCounter += 24
        # remove empty lines and seprate tags and instructions
        for lineno, line in enumerate(self.rawText, start=1):
            # skip empty lines
            if not line: continue
            # decode
            codeObj = Block.decode(line, lineno)
            if codeObj.isDirec():
                if codeObj.isStartDirec():
                    self.startTag = codeObj.getDirecInfo()
                elif codeObj.isDataDirec():
                    self.section = ASM.DATA_SECTION
                elif codeObj.isCodeDirec():
                    self.section = ASM.CODE_SECTION
                else:
                    self.data.extend(codeObj.getDirecInfo())
                    self.dataCounter += codeObj.assumedLength
            elif codeObj.isTag():
                if self.section == ASM.CODE_SECTION:
                    self.codeTag[codeObj.getTagName()] = self.codeCounter
                    self.code.append(codeObj)
                else:
                    self.dataTag[codeObj.getTagName()] = self.dataCounter
            else:
                self.code.append(codeObj)
                self.codeCounter += codeObj.assumedLength
            # print progress
            printProgress(lineno, len(self.rawText), info='Decoding')
        # prepend initialization codes
        if _ := self.codeTag.get(self.startTag, None):
            self.code = [
                Block.decode(f'tail {self.startTag}', 0)
            ] + self.code
        else:
            raise RuntimeError(f'no starting tag defined.')

    def optimize(self) -> None:
        assumedAddress = ASM.DEFAULT_PC
        actualAddress = ASM.DEFAULT_PC
        assumedTagDict = {**self.codeTag, **self.dataTag}
        for idx, codeObj in enumerate(self.code, start=1):
            # optimize
            readdressingInfo = codeObj.optimize(assumedAddress, assumedTagDict, actualAddress)
            assumedAddress += codeObj.assumedLength
            # readdress (only code-tag left in this stage)
            self.codeTag.update(readdressingInfo)
            actualAddress += codeObj.actualLength
            # print progress
            printProgress(idx, len(self.code), info='Optimizing')
        # remap data tags
        self.dataTag = {tag: (addr + actualAddress) for tag, addr in self.dataTag.items()}
    
    def finalize(self) -> None:
        # finalize all codes
        for idx, codeObj in enumerate(self.code, start=1):
            codeObj.finalize({**self.codeTag, **self.dataTag})
            # print progress
            printProgress(idx, len(self.code), 'Finalizing')
    
    def encode(self) -> None:
        self.machineCode.clear()
        for idx, codeObj in enumerate(self.code, start=1):
            self.machineCode.extend(codeObj.encode())
            # print progress
            printProgress(idx, len(self.code), info='Encoding')
    
    def save(self) -> None:
        # prepare for output
        if not os.path.exists('../bin/'):
            os.mkdir('../bin/')
        # output binary file
        if (lst := self.machineCode + self.data):
            with open(f'../bin/{self.fileName}.bin', 'wb') as file:
                for idx, mc in enumerate(lst, start=1):
                    file.write(struct.pack('<I', mc))
                    printProgress(idx, len(lst), info='Saving binary')

    def printTagInfo(self) -> None:
        print('\n'.join(f'{tag=}\taddr={addr:#08X}' for tag, addr in {**self.codeTag, **self.dataTag}.items()))
    
    def printDetailedInfo(self) -> None:
        print('\n'.join(str(codeObj) for codeObj in self.code))

if __name__ == '__main__':
    # parse arguments
    parser = argparse.ArgumentParser(description='assembler for risc-v')
    parser.add_argument('files', nargs='+', help='relative path to .s file needed, multiple files supported')
    parser.add_argument('--tags', action='store_true', required=False, help='print out tags')
    parser.add_argument('--verbose', action='store_true', required=False, help='print out detailed information')
    args = parser.parse_args()
    # process asm code
    try:
        asm = ASM()
        asm.load(args.files)
        asm.decode()
        asm.optimize()
        asm.finalize()
        asm.encode()
        asm.save()

        if args.tags: asm.printTagInfo()
        if args.verbose: asm.printDetailedInfo()
    except RuntimeError as e:
        print(e)
