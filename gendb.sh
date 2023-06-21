#!/bin/sh
touch sush.sqlite3 && cat schema.sql | sqlite3 sush.sqlite3

