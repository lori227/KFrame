#!/usr/bin/python
# -*- coding: UTF-8 -*-

import pymysql


def connect_mysql_db():
    return pymysql.connect(host='192.168.1.9',
                           port=3306,
                           user='root',
                           password='mysql@lori227-=',
                           database='kframe',
                           charset='utf8')


def query_mysql_db(sql):
    con = connect_mysql_db()
    cur = con.cursor()
    cur.execute(sql)
    rows = cur.fetchall()
    cur.close()
    con.close()

    assert len(rows) == 1, 'Fatal error: sql query failed, sql = %s' % sql
    return rows


def insert_mysql_db(sql):
    print sql
    con = connect_mysql_db()
    cur = con.cursor()
    try:
        cur.execute(sql)
        con.commit()
    except:
        con.rollback()
        raise
    finally:
        cur.close()
        con.close()


def update_mysql_db(sql):
    con = connect_mysql_db()
    cur = con.cursor()
    try:
        cur.execute(sql)
        con.commit()
    except:
        con.rollback()
        raise
    finally:
        cur.close()
        con.close()
