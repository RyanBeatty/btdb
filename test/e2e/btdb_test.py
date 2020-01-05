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
    proc.stdin.write(b"select bar from foo;\n")
    proc.stdin.write(b"select bar, baz from foo where baz = true;\n")
    proc.stdin.write(b"select bar, baz from foo where bar = 'world';\n")
    proc.stdin.write(b"select bar, baz from foo where bar = 'foo';\n")
    proc.stdin.write(b"select a from b;\n")
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
        btdb>     bar
        ===============
        hello\t
        world\t
        btdb>     bar    baz
        ===============
        hello\ttrue\t
        btdb>     bar    baz
        ===============
        world\tfalse\t
        btdb>     bar    baz
        ===============
        btdb>     a
        ===============
        asdf\t
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
    proc.stdin.write(b"insert into b (a) values ('c');\n")
    proc.stdin.write(b"select a from b;\n")
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
        btdb>     bar    baz
        ===============
        hello\ttrue\t
        world\tfalse\t
        a\tfalse\t
        b\ttrue\t
        btdb>     a
        ===============
        btdb>     a
        ===============
        asdf\t
        c\t
        btdb> Shutting down btdb
        """
        ),
        encoding="utf8",
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
    proc.stdin.write(b"delete from foo where bar = 'world';\n")
    proc.stdin.write(b"select bar, baz from foo;\n")
    proc.stdin.write(b"delete from b;\n")
    proc.stdin.write(b"select a from b;\n")
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
        btdb> ===============
        btdb>     bar    baz
        ===============
        world\tfalse\t
        btdb> ===============
        btdb>     bar    baz
        ===============
        btdb> ===============
        btdb>     a
        ===============
        btdb> Shutting down btdb
        """
        ),
        encoding="utf8",
    )

    proc.kill()


def test_delete_everything():
    proc = subprocess.Popen(
        ["./bin/btdb"],
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )

    proc.stdin.write(b"delete from foo;\n")
    proc.stdin.write(b"select bar, baz from foo;\n")
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
        btdb> ===============
        btdb>     bar    baz
        ===============
        btdb> Shutting down btdb
        """
        ),
        encoding="utf8",
    )

    proc.kill()


def test_update():
    proc = subprocess.Popen(
        ["./bin/btdb"],
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )

    proc.stdin.write(b"update foo set bar = 'a';\n")
    proc.stdin.write(b"select bar, baz from foo;\n")
    proc.stdin.write(b"update foo set bar = 'b' where baz = true;\n")
    proc.stdin.write(b"select bar, baz from foo;\n")
    proc.stdin.write(b"update b set a = 'updated';\n")
    proc.stdin.write(b"select a from b;\n")
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
        btdb> ===============
        btdb>     bar    baz
        ===============
        a\ttrue\t
        a\tfalse\t
        btdb> ===============
        btdb>     bar    baz
        ===============
        b\ttrue\t
        a\tfalse\t
        btdb> ===============
        btdb>     a
        ===============
        updated\t
        btdb> Shutting down btdb
        """
        ),
        encoding="utf8",
    )

    proc.kill()

def test_sort():
    proc = subprocess.Popen(
        ["./bin/btdb"],
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )

    proc.stdin.write(b"select bar, baz from foo order by bar;\n")
    proc.stdin.write(b"select bar, baz from foo order by bar desc;\n")
    proc.stdin.write(b"select bar, baz from foo order by baz;\n")
    proc.stdin.write(b"select bar, baz from foo order by baz desc;\n")
    proc.stdin.write(b"select bar, baz from foo order by foo;\n")
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
        world\tfalse\t
        hello\ttrue\t
        btdb>     bar    baz
        ===============
        world\tfalse\t
        hello\ttrue\t
        btdb>     bar    baz
        ===============
        hello\ttrue\t
        world\tfalse\t
        btdb> Query not valid
        btdb> Shutting down btdb
        """
        ),
        encoding="utf8",
    )

    proc.kill()
