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
    proc.stdin.write(b"select a, true, 'hello' from b;\n")
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
        cab\t
        btdb>     a    ?column0?    ?column1?
        ===============
        asdf\ttrue\thello\t
        cab\ttrue\thello\t
        btdb> Shutting down btdb
        """
        ),
        encoding="utf8",
    )

    proc.kill()


def test_select_with_joins():
    proc = subprocess.Popen(
        ["./bin/btdb"],
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )

    proc.stdin.write(b"select bar, baz, a from foo, b;\n")
    proc.stdin.write(b"select bar, a from foo, b;\n")
    proc.stdin.write(b"select bar, baz, a from foo, b where a = 'cab';\n")
    proc.stdin.write(b"select bar, baz, a from foo, b order by a;\n")

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
        btdb>     bar    baz    a
        ===============
        hello\ttrue\tasdf\t
        hello\ttrue\tcab\t
        world\tfalse\tasdf\t
        world\tfalse\tcab\t
        btdb>     bar    a
        ===============
        hello\tasdf\t
        hello\tcab\t
        world\tasdf\t
        world\tcab\t
        btdb>     bar    baz    a
        ===============
        hello\ttrue\tcab\t
        world\tfalse\tcab\t
        btdb>     bar    baz    a
        ===============
        hello\ttrue\tasdf\t
        world\tfalse\tasdf\t
        hello\ttrue\tcab\t
        world\tfalse\tcab\t
        btdb> Shutting down btdb
        """
        ),
        encoding="utf8",
    )

    proc.kill()

def test_integer_support():
    proc = subprocess.Popen(
        ["./bin/btdb"],
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )

    proc.stdin.write(b"create table baz (foo int);\n")
    proc.stdin.write(b"insert into baz (foo) values (1), (23), (-5), (0), (-0);\n")
    proc.stdin.write(b"select foo from baz;\n")
    proc.stdin.write(b"select foo from baz where foo > -1;\n")
    proc.stdin.write(b"select foo from baz order by foo;\n")
    proc.stdin.write(b"select foo + 1 from baz;\n")
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
        btdb> UTILITY DONE
        btdb>     foo
        ===============
        btdb>     foo
        ===============
        1\t
        23\t
        -5\t
        0\t
        0\t
        btdb>     foo
        ===============
        1\t
        23\t
        0\t
        0\t
        btdb>     foo
        ===============
        -5\t
        0\t
        0\t
        1\t
        23\t
        btdb>     ?column0?
        ===============
        2\t
        24\t
        -4\t
        1\t
        1\t
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
        cab\t
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


def test_create_table():
    proc = subprocess.Popen(
        ["./bin/btdb"],
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )

    proc.stdin.write(b"create table baz (id text, boq bool);\n")
    proc.stdin.write(b"insert into baz (id, boq) values ('hello', true), ('world', false);\n")
    proc.stdin.write(b"select id, boq from baz;\n")
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
        btdb> UTILITY DONE
        btdb>     id    boq
        ===============
        btdb>     id    boq
        ===============
        hello\ttrue\t
        world\tfalse\t
        btdb> Shutting down btdb
        """
        ),
        encoding="utf8",
    )

    proc.kill()
