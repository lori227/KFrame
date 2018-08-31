#!/usr/bin/python
# -*- coding: UTF-8 -*-

import pymysql

class db_info:
    def __init__(self, h, p, u, pwd, db):
        self.host = h
        self.port = p
        self.user = u
        self.password = pwd
        self.database = db
        self.charset = 'utf8'

def connect_mysql_db(mysql_info):
    return pymysql.connect(host=mysql_info.host,
                           port=mysql_info.port,
                           user=mysql_info.user,
                           password=mysql_info.password,
                           database=mysql_info.database,
                           charset=mysql_info.charset)


def query_mysql_db(mysql_info, sql):
    con = connect_mysql_db(mysql_info)
    cur = con.cursor()
    cur.execute(sql)
    rows = cur.fetchall()
    cur.close()
    con.close()

    assert len(rows) == 1, 'Fatal error: sql query failed, sql = %s' % sql
    return rows


def insert_mysql_db(mysql_info, sql):
    print sql
    con = connect_mysql_db(mysql_info)
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


def update_mysql_db(mysql_info, sql):
    con = connect_mysql_db(mysql_info)
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
