#!/usr/bin/env python3

import argparse
import json
import os
import subprocess
import sys

def run_tidy(clang_tidy_binary, e):
    f = e['file']
    cmd = e['command']
    print('Checking {}'.format(f))
    invocation = [clang_tidy_binary, '--quiet', '-extra-arg=-Wno-unknown-warning-option', f, '--', '-fms-extensions', '-m32'] + cmd.split()
    subprocess.check_call(invocation)

def main():
    parser = argparse.ArgumentParser(description='Runs clang-tidy over all files '
        'in a compilation database. Requires clang-tidy in $PATH.')
    parser.add_argument('-clang-tidy-binary', metavar='PATH',
        default='clang-tidy', help='path to clang-tidy binary')
    parser.add_argument('-p', dest='build_path',
        help='Path used to read a compile command database.')
    parser.add_argument('-quiet', action='store_true',
        help='Run clang-tidy in quiet mode')
    args = parser.parse_args()

    db_path = 'compile_commands.json'

    if args.build_path is not None:
        build_path = args.build_path
    else:
        print("Please specify build path.")
        sys.exit(1)

    try:
        invocation = [args.clang_tidy_binary, '-list-checks']
        invocation.append('-p=' + build_path)
        with open(os.devnull, 'w') as dev_null:
            subprocess.check_call(invocation, stdout=dev_null)
    except:
        print("Unable to run clang-tidy.", file=sys.stderr)
        sys.exit(1)

    database = json.load(open(os.path.join(build_path, db_path)))
    files = [entry['file'] for entry in database]
    common_path = os.path.commonpath(files)

    for e in database:
        f = e['file']
        if f.endswith('.cpp'):
            rel_path = os.path.relpath(f, common_path)
            if os.path.commonpath(['app/', rel_path]):
                run_tidy(args.clang_tidy_binary, e)
            elif os.path.commonpath(['lib/utmsrpc', rel_path]):
                run_tidy(args.clang_tidy_binary, e)

if __name__ == '__main__':
  main()
