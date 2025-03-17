#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import codecs
import logging
import pathlib


def convert_encoding(root_dir, src_encoding='utf-8', dst_encoding='utf-8-sig'):
    for root, _, files in os.walk(root_dir):
        path_root = pathlib.Path(root)
        for file in files:
            if file.endswith('.cpp') or file.endswith('.hpp'):
                file_path = path_root / file
                # 读取
                try:
                    with codecs.open(file_path, 'r', encoding=src_encoding) as f:
                        content = f.read()
                except UnicodeDecodeError:
                    logging.error(f'按 {src_encoding} 编码，从文件 [{str(file_path)}] 读取内容时，发生错误!')
                # 转换
                try:
                    with codecs.open(file_path, 'w', encoding=dst_encoding) as f:
                        f.write(content)
                except UnicodeDecodeError:
                    logging.error(f'按 {dst_encoding} 编码，从文件 [{str(file_path)}] 写入内容时，发生错误!')


def show_file_dialog():
    import tkinter as tk
    from tkinter import filedialog
    folder_path = filedialog.askdirectory(initialdir=os.curdir)
    if folder_path and os.path.isdir(folder_path):
        convert_encoding(folder_path)


if __name__ == '__main__':
    show_file_dialog()
