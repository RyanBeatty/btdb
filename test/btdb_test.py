import shlex
import subprocess
import textwrap

import pytest

PROMPT = b"btdb> "
START_MSG = b"Starting btdb\n" + PROMPT
SHUTDOWN_MSG = PROMPT + b"Shutting down btdb\n"


def test_select():
    proc = subprocess.Popen(
        ["./bin/btdb"],
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )

    proc.stdin.write(b"select bar, baz from foo;\n")
    proc.stdin.write(b"select bar, baz from foo where baz = true;\n")
    proc.stdin.write(b"select bar, baz from foo where bar = 'world';\n")
    proc.stdin.write(b"select bar, baz from foo where bar = 'foo';\n")
    try:
        output, err = proc.communicate(timeout=2)
    except subprocess.TimeoutExpired:
        proc.kill()
        proc.communicate()
        assert False

    assert not err
    assert output == bytes(
        textwrap.dedent(
            f"""\
        Starting btdb
        btdb>     bar    baz
        ===============
        hello\ttrue\t
        world\tfalse\t
        btdb>     bar    baz
        ===============
        hello\ttrue\t
        btdb>     bar    baz
        ===============
        world\tfalse\t
        btdb>     bar    baz
        ===============
        btdb> Shutting down btdb
        """
        ),
        encoding="utf8",
    )

    proc.kill()


def test_insert():
    proc = subprocess.Popen(
        ["./bin/btdb"],
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )

    proc.stdin.write(b"insert into foo (bar, baz) values ('a', false), ('b', true);\n")
    proc.stdin.write(b"select bar, baz from foo;\n")
    try:
        output, err = proc.communicate(timeout=2)
    except subprocess.TimeoutExpired:
        proc.kill()
        proc.communicate()
        assert False

    assert not err
    assert output == (
        START_MSG + b"    bar    baz\n"
        b"===============\n\t\t\n\t\t\n" + PROMPT + b"    bar    baz\n"
        b"===============\n"
        b"hello\ttrue\t\n"
        b"world\tfalse\t\n"
        b"b\ttrue\t\n"
        b"a\tfalse\t\n" + SHUTDOWN_MSG
    )

    proc.kill()


def test_delete():
    proc = subprocess.Popen(
        ["./bin/btdb"],
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )

    proc.stdin.write(b"delete from foo where baz = true;\n")
    proc.stdin.write(b"select bar, baz from foo;\n")
    try:
        output, err = proc.communicate(timeout=2)
    except subprocess.TimeoutExpired:
        proc.kill()
        proc.communicate()
        assert False

    assert not err
    assert output == (
        START_MSG + b"    bar    baz\n"
        b"===============\n" + PROMPT + b"    bar    baz"
        b"==============="
        b"world\tfalse\t\n" + SHUTDOWN_MSG
    )

    proc.kill()
