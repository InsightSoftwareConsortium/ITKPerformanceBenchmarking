#!/usr/bin/env python

import argparse
import subprocess
import sys
import os

parser = argparse.ArgumentParser(prog='evaluate-itk-performance')

subparsers = parser.add_subparsers(help='subcommands for individual steps')

run_parser = subparsers.add_parser('run',
        help='build ITK and build and run the benchmarks')
run_parser.add_argument('src', help='ITK source directory')
run_parser.add_argument('bin', help='ITK build directory')
run_parser.add_argument('benchmark_src',
        help='ITK performance benchmarks source directory')
run_parser.add_argument('benchmark_bin',
        help='ITK performance benchmarks build directory')
run_parser.add_argument('-g', '--git-tag',
        help='ITK Git tag', default='master')

args = parser.parse_args()

def check_for_required_programs():
    try:
        subprocess.check_call(['git', '--version'], stdout=subprocess.PIPE)
    except subprocess.CalledProcessError:
        sys.stderr.write("Could not run 'git', please install Git\n")
        sys.exit(1)
    try:
        subprocess.check_call(['cmake', '--version'], stdout=subprocess.PIPE)
    except CalledProcessError:
        sys.stderr.write("Could not run 'cmake', please install CMake\n")
        sys.exit(1)
    try:
        subprocess.check_call(['ctest', '--version'], stdout=subprocess.PIPE)
    except CalledProcessError:
        sys.stderr.write("Could not run 'ctest', please install CMake\n")
        sys.exit(1)
    try:
        subprocess.check_call(['ninja', '--version'], stdout=subprocess.PIPE)
    except CalledProcessError:
        sys.stderr.write("Could not run 'ninja', please install the Ninja build tool\n")
        sys.exit(1)

def create_run_directories(itk_src, itk_bin, benchmark_src, benchmark_bin, git_tag):
    if not os.path.exists(os.path.join(itk_src, '.git')):
        dirname = os.path.dirname(itk_src)
        if not os.path.exists(dirname):
            os.makedirs(dirname)
        subprocess.check_call(['git', 'clone',
            'https://github.com/InsightSoftwareConsortium/ITK.git', itk_src])
    os.chdir(itk_src)
    # Stash any uncommited changes
    subprocess.check_call(['git', 'stash'])
    subprocess.check_call(['git', 'reset', '--hard', git_tag])

    if not os.path.exists(itk_bin):
        os.makedirs(itk_bin)

    if not os.path.exists(os.path.join(benchmark_src, '.git')):
        dirname = os.path.dirname(benchmark_src)
        if not os.path.exists(dirname):
            os.makedirs(dirname)
        subprocess.check_call(['git', 'clone',
            'https://github.com/InsightSoftwareConsortium/ITKPerformanceBenchmarking.git', benchmark_src])

    if not os.path.exists(benchmark_bin):
        os.makedirs(benchmark_bin)

def extract_itk_information(itk_src):
    information = dict()
    os.chdir(itk_src)
    itk_git_sha = subprocess.check_output(['git', 'rev-parse', 'HEAD']).strip()
    information['ITK_GIT_SHA'] = itk_git_sha
    itk_git_date = subprocess.check_output(['git', 'show', '-s', '--format=%ci',
        'HEAD']).strip()
    information['ITK_GIT_DATE'] = itk_git_date
    local_modifications = subprocess.check_output(['git', 'diff', '--shortstat',
            'HEAD'])
    information['ITK_GIT_LOCAL_MODIFICATIONS'] = local_modifications
    print(local_modifications)
    return information

def build_itk(itk_src, itk_bin):
    os.chdir(itk_bin)
    subprocess.check_call(['cmake',
        '-G', 'Ninja',
        '-DCMAKE_BUILD_TYPE:STRING=Release',
        '-DCMAKE_CXX_STANDARD:STRING=11',
        '-DBUILD_TESTING:BOOL=OFF',
        '-DBUILD_EXAMPLES:BOOL=OFF',
        itk_src])
    subprocess.check_call(['ninja'])

# fca883daf05ac62ee0449513dbd2ad30ff9591f0 is sha1 that introduces itk::BuildInformation
# so all ancestors need to prevent the benchmarking from using
def check_for_build_information(itk_src):
    os.chdir(itk_src)
    try:
        subprocess.check_call(['git', 'merge-base',
            '--is-ancestor', 'HEAD',
            'fca883daf05ac62ee0449513dbd2ad30ff9591f0']).strip()
        has_itkbuildinformation = False
    except subprocess.CalledProcessError:
        has_itkbuildinformation = True
    return has_itkbuildinformation

def build_benchmarks(benchmark_src, benchmark_bin,
        itk_bin,
        itk_has_buildinformation):
    os.chdir(benchmark_bin)
    if itk_has_buildinformation:
        build_information_arg = '-DITK_HAS_INFORMATION_H:BOOL=ON'
    else:
        build_information_arg = '-DITK_HAS_INFORMATION_H:BOOL=OFF'
    subprocess.check_call(['cmake',
        '-G', 'Ninja',
        '-DCMAKE_BUILD_TYPE:STRING=Release',
        '-DCMAKE_CXX_STANDARD:STRING=11',
        '-DITK_DIR:PATH=' + itk_bin,
        build_information_arg,
        benchmark_src])
    subprocess.check_call(['ninja'])

def run_benchmarks(benchmark_bin):
    os.chdir(benchmark_bin)
    subprocess.check_call(['ctest'])

check_for_required_programs()

create_run_directories(args.src, args.bin,
        args.benchmark_src, args.benchmark_bin,
        args.git_tag)

print('\n\nITK Repository Information:')
itk_information = extract_itk_information(args.src)
print(itk_information)


print('\nBuilding ITK...')
build_itk(args.src, args.bin)

itk_has_buildinformation = check_for_build_information(args.src)

print('\nBuilding benchmarks...')
build_benchmarks(args.benchmark_src, args.benchmark_bin, args.bin,
        itk_has_buildinformation)

print('\nRunning benchmarks...')
run_benchmarks(args.benchmark_bin)

print('\nDone running performance benchmarks.')
