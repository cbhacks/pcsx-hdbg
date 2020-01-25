//
// PCSX-HDBG - PCSX-based hack debugger
// Copyright (C) 2019-2020  "chekwob" <chek@wobbyworks.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    if (argc != 4) {
        fprintf(stderr, "usage: <infile> <outfile> <var-prefix>\n");
        fprintf(stderr, "    infile:      input binary file\n");
        fprintf(stderr, "    outfile:     output C source file\n");
        fprintf(stderr, "    var-prefix:  prefix for variables\n");
        fprintf(stderr, "\n");
        fprintf(stderr, "    generated source has two variables:\n");
        fprintf(stderr, "        const char <var-prefix>_data[];\n");
        fprintf(stderr, "        const size_t <var-prefix>_size;\n");
        return EXIT_FAILURE;
    }

    const char *infile = argv[1];
    const char *outfile = argv[2];
    const char *var_prefix = argv[3];

    FILE *inF = fopen(infile, "rb");
    if (!inF) {
        perror(infile);
        return EXIT_FAILURE;
    }

    FILE *outF = fopen(outfile, "w");
    if (!outF) {
        perror(outfile);
        fclose(inF);
        return EXIT_FAILURE;
    }

    fprintf(outF, "#include <stddef.h>\n");
    fprintf(outF, "\n");
    fprintf(outF, "const char %s_data[] = \"", var_prefix);

    int c;
    while ((c = fgetc(inF)) != EOF) {
        fprintf(outF, "\\x%X", c);
    }

    fprintf(outF, "\";\n");
    fprintf(outF,
        "const size_t %s_size = sizeof(%s_data) - 1;\n",
        var_prefix,
        var_prefix
    );

    if (ferror(inF)) {
        perror(infile);
        fclose(inF);
        fclose(outF);
        return EXIT_FAILURE;
    }
    fclose(inF);

    if (ferror(outF)) {
        perror(outfile);
        fclose(outF);
        return EXIT_FAILURE;
    }
    fclose(outF);

    return EXIT_SUCCESS;
}
