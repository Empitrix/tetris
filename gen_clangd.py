import os

IS_LINUX:bool = os.name == "posix";

CWD:str = os.getcwd()
CLANGD:str = os.path.join(CWD, ".clangd")

CLANGD_COMPILER:str = "gcc"

CLANGD_COMPILE_FLAGS:list[str] = [

]

CLANGD_LIBRARY_DIRECTORIES:list[str] = [
	f"{ CWD }/support/win32/lib"
]

CLANGD_INCLUDE_DIRECTORIES:list[str] = [
	f"{ CWD }/support/win32/include"
]


TEMPLATE:str = """\
CompileFlags:
	Compiler: @@COMPILER@@
	Add:
@@FLAGS@@
"""

for idx, itm in enumerate(CLANGD_COMPILE_FLAGS):
	CLANGD_COMPILE_FLAGS[idx] = "\t\t- " + itm + "\n"

for idx, itm in enumerate(CLANGD_INCLUDE_DIRECTORIES):
	CLANGD_INCLUDE_DIRECTORIES[idx] = "\t\t- " + '\"' + "-I" + itm + '\"' + "\n"

for idx, itm in enumerate(CLANGD_LIBRARY_DIRECTORIES):
	CLANGD_LIBRARY_DIRECTORIES[idx] = "\t\t- " + '\"' + "-L" + itm + '\"' + "\n"


TEMPLATE = TEMPLATE.replace("@@COMPILER@@", CLANGD_COMPILER)
TEMPLATE = TEMPLATE.replace("@@FLAGS@@", f"{"".join(CLANGD_INCLUDE_DIRECTORIES) + "".join(CLANGD_LIBRARY_DIRECTORIES) + "".join(CLANGD_COMPILE_FLAGS)}");
TEMPLATE = TEMPLATE.replace("\\", "/")
TEMPLATE = TEMPLATE.replace("\t", "  ")
TEMPLATE = TEMPLATE.strip();

# print(TEMPLATE);

with open(CLANGD, "w+") as clangd:
	clangd.write(TEMPLATE);

