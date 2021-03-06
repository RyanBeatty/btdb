import os

import shlex
import subprocess
import textwrap

import pytest

PROMPT = b"btdb> "
START_MSG = b"Starting btdb\n" + PROMPT
SHUTDOWN_MSG = PROMPT + b"Shutting down btdb\n"
# TODO: Using this script to start the db each time breaks debug mode. gdb can't load the symbol
# table because of the other commands I think. As a short term fix, you can change this statement 
# to ./build/apps/btdb to get around this.
BTDB_BIN_PATH = "./build/apps/btdb"
DEBUG = os.getenv("DEBUG", None) is not None
TIMEOUT = None if DEBUG else 2


def _start_btdb_process(clean_start=True):
    start_cmd = [BTDB_BIN_PATH]
    if clean_start:
        os.system('cd build && make reset_data_dir && cd ..')
    proc = subprocess.Popen(
        start_cmd,
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        bufsize=0,
    )

    if DEBUG:
        print(f"btdb pid: {proc.pid}")
        input("press enter to continue")
    return proc


def test_select():
    proc = _start_btdb_process()

    input_cmds = bytes(
        textwrap.dedent(
            """\
        select bar, baz from foo;
        select bar from foo;
        select bar, baz from foo where baz = true;
        select bar, baz from foo where bar = 'world';
        select bar, baz from foo where bar = 'foo';
        select a from b;
        select a, true, 'hello' from b;
        """
        ),
        encoding="utf-8",
    )
    try:
        output, err = proc.communicate(input=input_cmds, timeout=TIMEOUT)
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
        [BTDB_BIN_PATH],
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )

    input_cmds = bytes(
        textwrap.dedent(
            """\
        select bar, baz, a from foo, b;
        select bar, a from foo, b;
        select bar, baz, a from foo, b where a = 'cab';
        select bar, baz, a from foo, b order by a;
        select bar, baz, a from foo join b on a = bar;
        select bar, baz, a from foo left join b on a = bar;
        select bar, baz, a from foo right join b on a = bar;
        insert into b (a) values ('hello');
        select bar, baz, a from foo join b on a = bar;
        select bar, baz, a from foo left join b on a = bar;
        select bar, baz, a from foo right join b on a = bar;
        """
        ),
        encoding="utf-8",
    )

    try:
        output, err = proc.communicate(input=input_cmds, timeout=TIMEOUT)
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
        btdb>     bar    baz    a
        ===============
        btdb>     bar    baz    a
        ===============
        hello\ttrue\t\t\t
        world\tfalse\t\t\t
        btdb>     bar    baz    a
        ===============
        \t\t\t\tasdf\t
        \t\t\t\tcab\t
        btdb>     a
        ===============
        btdb>     bar    baz    a
        ===============
        hello\ttrue\thello\t
        btdb>     bar    baz    a
        ===============
        hello\ttrue\thello\t
        world\tfalse\t\t\t
        btdb>     bar    baz    a
        ===============
        \t\t\t\tasdf\t
        \t\t\t\tcab\t
        hello\ttrue\thello\t
        btdb> Shutting down btdb
        """
        ),
        encoding="utf8",
    )

    proc.kill()


def test_integer_support():
    proc = _start_btdb_process()

    input_cmds = bytes(
        textwrap.dedent(
            """\
        create table baz (foo int);
        insert into baz (foo) values (1), (23), (-5), (0), (-0);
        select foo from baz;
        select foo from baz where foo > -1;
        select foo from baz order by foo;
        select foo + 1 from baz;
        """
        ),
        encoding="utf-8",
    )
    try:
        output, err = proc.communicate(input=input_cmds, timeout=TIMEOUT)
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
    proc = _start_btdb_process()

    input_cmds = bytes(
        textwrap.dedent(
            """\
        insert into foo (bar, baz) values ('a', false), ('b', true);
        select bar, baz from foo;
        insert into b (a) values ('c');
        select a from b;
        insert into foo (bar, baz) values ('d', true and false);
        select bar, baz from foo;
        insert into foo (bar, baz) values ('e', null);
        select bar, baz from foo;
        """
        ),
        encoding="utf-8",
    )
    try:
        output, err = proc.communicate(input=input_cmds, timeout=TIMEOUT)
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
        btdb>     bar    baz
        ===============
        btdb>     bar    baz
        ===============
        hello\ttrue\t
        world\tfalse\t
        a\tfalse\t
        b\ttrue\t
        d\tfalse\t
        btdb>     bar    baz
        ===============
        btdb>     bar    baz
        ===============
        hello\ttrue\t
        world\tfalse\t
        a\tfalse\t
        b\ttrue\t
        d\tfalse\t
        e\t\t\t
        btdb> Shutting down btdb
        """
        ),
        encoding="utf8",
    )

    proc.kill()


def test_large_insert():
    proc = _start_btdb_process()

    input_cmds = []
    expected_output = ["Starting btdb"]
    for _ in range(800):
        input_cmds.append("insert into foo (bar, baz) values ('hello world', true);\n")
        expected_output.append("btdb>     bar    baz")
        expected_output.append("===============")
    input_cmds.append("select bar, baz from foo;")
    expected_output.append("btdb>     bar    baz")
    expected_output.append("===============")
    # These rows are already pre populated.
    expected_output.append("hello\ttrue\t")
    expected_output.append("world\tfalse\t")
    for _ in range(800):
        expected_output.append("hello world\ttrue\t")
    expected_output.append("btdb> Shutting down btdb\n")
    try:
        output, err = proc.communicate(
            input=bytes("".join(input_cmds), encoding="utf-8"), timeout=TIMEOUT
        )
    except subprocess.TimeoutExpired:
        proc.kill()
        proc.communicate()
        assert False

    assert not err
    assert output == bytes("\n".join(expected_output), encoding="utf8")

    proc.kill()


def test_delete():
    proc = _start_btdb_process()

    input_cmds = bytes(
        textwrap.dedent(
            """\
        delete from foo where baz = true;
        select bar, baz from foo;
        delete from foo where bar = 'world';
        select bar, baz from foo;
        delete from b;
        select a from b;
        """
        ),
        encoding="utf-8",
    )
    try:
        output, err = proc.communicate(input=input_cmds, timeout=TIMEOUT)
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
    proc = _start_btdb_process()

    input_cmds = bytes(
        textwrap.dedent(
            """\
        delete from foo;
        select bar, baz from foo;
        """
        ),
        encoding="utf-8",
    )
    try:
        output, err = proc.communicate(input=input_cmds, timeout=TIMEOUT)
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
    proc = _start_btdb_process()

    input_cmds = bytes(
        textwrap.dedent(
            """\
        update foo set bar = 'a';
        select bar, baz from foo;
        update foo set bar = 'b' where baz = true;
        select bar, baz from foo;
        update b set a = 'updated';
        select a from b;
        """
        ),
        encoding="utf-8",
    )
    try:
        output, err = proc.communicate(input=input_cmds, timeout=TIMEOUT)
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
        a\tfalse\t
        b\ttrue\t
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
    proc = _start_btdb_process()

    input_cmds = bytes(
        textwrap.dedent(
            """\
        select bar, baz from foo order by bar;
        select bar, baz from foo order by bar desc;
        select bar, baz from foo order by baz;
        select bar, baz from foo order by baz desc;
        select bar, baz from foo order by foo;
        """
        ),
        encoding="utf-8",
    )
    try:
        output, err = proc.communicate(input=input_cmds, timeout=TIMEOUT)
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
    proc = _start_btdb_process()

    input_cmds = bytes(
        textwrap.dedent(
            """\
        create table baz (id text, boq bool);
        insert into baz (id, boq) values ('hello', true), ('world', false);
        select id, boq from baz;
        """
        ),
        encoding="utf-8",
    )
    try:
        output, err = proc.communicate(input=input_cmds, timeout=TIMEOUT)
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

def test_create_table_durable():
    """
    Clean start, create table + insert, shutdown, restart.
    """
    # Clean start. Create table insert some rows.
    proc = _start_btdb_process()

    input_cmds = bytes(
        textwrap.dedent(
            """\
        create table baz (x text, y bool, z int);
        insert into baz (x, y, z) values ('hello', true, 1), ('world', false, 2);
        select x, y, z from baz;
        """
        ),
        encoding="utf-8",
    )
    try:
        output, err = proc.communicate(input=input_cmds, timeout=TIMEOUT)
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
        btdb>     x    y    z
        ===============
        btdb>     x    y    z
        ===============
        hello\ttrue\t1\t
        world\tfalse\t2\t
        btdb> Shutting down btdb
        """
        ),
        encoding="utf8",
    )
    proc.kill()

    # Select rows from table we created last start. Create a new table and insert rows into that new table.
    proc = _start_btdb_process(clean_start=False)

    input_cmds = bytes(
        textwrap.dedent(
            """\
        select x, y, z from baz;
        create table zed (v text);
        insert into zed (v) values ('zed'), ('dead');
        select v from zed where v = 'dead';
        """
        ),
        encoding="utf-8",
    )
    try:
        output, err = proc.communicate(input=input_cmds, timeout=TIMEOUT)
    except subprocess.TimeoutExpired:
        proc.kill()
        proc.communicate()
        assert False

    assert not err
    assert output == bytes(
        textwrap.dedent(
            f"""\
        Starting btdb
        btdb>     x    y    z
        ===============
        hello\ttrue\t1\t
        world\tfalse\t2\t
        btdb> UTILITY DONE
        btdb>     v
        ===============
        btdb>     v
        ===============
        dead\t
        btdb> Shutting down btdb
        """
        ),
        encoding="utf8",
    )
    proc.kill()

    # Read the rows from the table we created last start.
    proc = _start_btdb_process(clean_start=False)

    input_cmds = bytes(
        textwrap.dedent(
            """\
        select v from zed;
        """
        ),
        encoding="utf-8",
    )
    try:
        output, err = proc.communicate(input=input_cmds, timeout=TIMEOUT)
    except subprocess.TimeoutExpired:
        proc.kill()
        proc.communicate()
        assert False

    assert not err
    assert output == bytes(
        textwrap.dedent(
            f"""\
        Starting btdb
        btdb>     v
        ===============
        zed\t
        dead\t
        btdb> Shutting down btdb
        """
        ),
        encoding="utf8",
    )
    proc.kill()


def test_create_index():
    """
    Simple test to verify we can create an index on a table and use it in a scan. assumes that the order of
    results are in index order.
    """
    proc = _start_btdb_process()

    input_cmds = bytes(
        textwrap.dedent(
            """\
        create table c (d int);
        insert into c (d) values (4), (3), (2), (1);
        create index on c (d);
        select d from c where d >= 1;
        """
        ),
        encoding="utf-8",
    )
    try:
        output, err = proc.communicate(input=input_cmds, timeout=TIMEOUT)
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
        btdb>     d
        ===============
        btdb> UTILITY DONE
        btdb>     d
        ===============
        1\t
        2\t
        3\t
        4\t
        btdb> Shutting down btdb
        """
        ),
        encoding="utf8",
    )

    proc.kill()


def test_create_index_page_splits_in_order():
    """
    Verify that we can create an index on a table who's data will take up more than a single page.
    Note that we only test situations where the next tuple to be inserted occurs at the end of
    the page.
    """
    proc = _start_btdb_process()

    input_cmds = []
    expected_output = ["Starting btdb"]
    input_cmds.append('create table c (d text);\n')
    expected_output.append('btdb> UTILITY DONE')
    for i in range(10 ** 15, 10 ** 15 + 400):
        input_cmds.append(f"insert into c (d) values ('{str(i)}');\n")
        expected_output.append("btdb>     d")
        expected_output.append("===============")
    input_cmds.append("create index on c (d);\n")
    expected_output.append('btdb> UTILITY DONE')
    input_cmds.append("select d from c where d > '1';")
    expected_output.append("btdb>     d")
    expected_output.append("===============")
    for i in range(10 ** 15, 10 ** 15 + 400):
        expected_output.append(f"{str(i)}\t")
    expected_output.append("btdb> Shutting down btdb\n")
    try:
        output, err = proc.communicate(
            input=bytes("".join(input_cmds), encoding="utf-8"), timeout=TIMEOUT
        )
    except subprocess.TimeoutExpired:
        proc.kill()
        proc.communicate()
        assert False

    assert not err
    assert output == bytes("\n".join(expected_output), encoding="utf8")

    proc.kill()


def test_create_index_page_splits_reverse_order():
    """
    Verify that we can create an index on a table who's data will take up more than a single page.
    We insert items in reverse order to verify that we can calculate which page to add the new item to.
    """
    proc = _start_btdb_process()

    input_cmds = []
    expected_output = ["Starting btdb"]
    input_cmds.append('create table c (d text);\n')
    expected_output.append('btdb> UTILITY DONE')
    # Insertion is in the reverse order, so each new tuple should be inserted at front of page.
    for i in range(10 ** 15 + 400 - 1, 10 ** 15 -1, -1):
        input_cmds.append(f"insert into c (d) values ('{str(i)}');\n")
        expected_output.append("btdb>     d")
        expected_output.append("===============")
    input_cmds.append("create index on c (d);\n")
    expected_output.append('btdb> UTILITY DONE')
    input_cmds.append("select d from c where d > '1';")
    expected_output.append("btdb>     d")
    expected_output.append("===============")
    for i in range(10 ** 15, 10 ** 15 + 400):
        expected_output.append(f"{str(i)}\t")
    expected_output.append("btdb> Shutting down btdb\n")
    try:
        output, err = proc.communicate(
            input=bytes("".join(input_cmds), encoding="utf-8"), timeout=TIMEOUT
        )
    except subprocess.TimeoutExpired:
        proc.kill()
        proc.communicate()
        assert False

    print(output)
    assert not err
    assert output == bytes("\n".join(expected_output), encoding="utf8")

    proc.kill()


def test_create_index_page_splits_uneven_row_size():
    """
    Verify that we can create an index on a table who's data will take up more than a single page.
    We first insert some rows with the same size. Then we insert a row with a much larger size that
    should be in the middle of the left page when splitting to test that we can calculate the split loc
    correctly.
    """
    proc = _start_btdb_process()

    input_cmds = []
    expected_output = ["Starting btdb"]
    input_cmds.append('create table c (d text);\n')
    expected_output.append('btdb> UTILITY DONE')
    for i in range(10 ** 15, 10 ** 15 + 100):
        input_cmds.append(f"insert into c (d) values ('{str(i)}');\n")
        expected_output.append("btdb>     d")
        expected_output.append("===============")
    for i in range(10 ** 15, 10 ** 15 + 19):
        j = str(i)
        j = '3' + j[1:]
        input_cmds.append(f"insert into c (d) values ('{j}');\n")
        expected_output.append("btdb>     d")
        expected_output.append("===============")
    # Insert very long string
    long_str = str(2 * 10 ** 1000)
    input_cmds.append(f"insert into c (d) values ('{long_str}');\n")
    expected_output.append("btdb>     d")
    expected_output.append("===============")
    input_cmds.append("create index on c (d);\n")
    expected_output.append('btdb> UTILITY DONE')
    input_cmds.append("select d from c where d > '1';")
    expected_output.append("btdb>     d")
    expected_output.append("===============")
    for i in range(10 ** 15, 10 ** 15 + 100):
        expected_output.append(f"{str(i)}\t")
    expected_output.append(f"{long_str}\t")
    for i in range(10 ** 15, 10 ** 15 + 19):
        j = str(i)
        j = '3' + j[1:]
        expected_output.append(f"{j}\t")
    expected_output.append("btdb> Shutting down btdb\n")
    try:
        output, err = proc.communicate(
            input=bytes("".join(input_cmds), encoding="utf-8"), timeout=TIMEOUT
        )
    except subprocess.TimeoutExpired:
        proc.kill()
        proc.communicate()
        assert False

    print(output)
    assert not err
    assert output == bytes("\n".join(expected_output), encoding="utf8")

    proc.kill()


def test_create_index_page_splits_in_order_large_final_item():
    """
    Verify that we can create an index on a table who's data will take up more than a single page.
    Note that we only test situations where the next tuple to be inserted occurs at the end of
    the page.
    """
    proc = _start_btdb_process()

    input_cmds = []
    expected_output = ["Starting btdb"]
    input_cmds.append('create table c (d text);\n')
    expected_output.append('btdb> UTILITY DONE')
    for i in range(10 ** 15, 10 ** 15 + 58):
        input_cmds.append(f"insert into c (d) values ('{str(i)}');\n")
        expected_output.append("btdb>     d")
        expected_output.append("===============")
    # Insert very long string thats about 3/4's the size of a page.
    long_str = '2' * (2 ** 12 + 2 ** 11)
    input_cmds.append(f"insert into c (d) values ('{long_str}');\n")
    expected_output.append("btdb>     d")
    expected_output.append("===============")
    input_cmds.append("create index on c (d);\n")
    expected_output.append('btdb> UTILITY DONE')
    input_cmds.append("select d from c where d > '1';")
    expected_output.append("btdb>     d")
    expected_output.append("===============")
    for i in range(10 ** 15, 10 ** 15 + 58):
        expected_output.append(f"{str(i)}\t")
    expected_output.append(f"{long_str}\t")
    expected_output.append("btdb> Shutting down btdb\n")
    try:
        output, err = proc.communicate(
            input=bytes("".join(input_cmds), encoding="utf-8"), timeout=TIMEOUT
        )
    except subprocess.TimeoutExpired:
        proc.kill()
        proc.communicate()
        assert False

    assert not err
    assert output == bytes("\n".join(expected_output), encoding="utf8")

    proc.kill()
