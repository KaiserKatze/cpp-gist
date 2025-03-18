#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import codecs
import logging
import pathlib


def convert_encoding(root_dir, src_encoding='gb2312', dst_encoding='utf-8-sig'):
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
                    continue
                # 转换
                try:
                    with codecs.open(file_path, 'w', encoding=dst_encoding) as f:
                        f.write(content)
                except UnicodeDecodeError:
                    logging.error(f'按 {dst_encoding} 编码，从文件 [{str(file_path)}] 写入内容时，发生错误!')


def show_directory_dialog():
    import tkinter as tk
    from tkinter import filedialog
    folder_path = filedialog.askdirectory(title='选择需要转换编码的文件', initialdir=os.curdir)
    if folder_path and os.path.isdir(folder_path):
        convert_encoding(folder_path)


def show_file_dialog():
    from tkinter import filedialog
    file_path = filedialog.askopenfilenames(title='选择需要转换编码的文件', initialdir=os.curdir, multiple=True)
    if not file_path:
        return
    file_path = [pathlib.Path(os.curdir) / file for file in file_path]
    if not all(file.is_file() for file in file_path):
        return
    for file in file_path:
        convert_encoding(file, src_encoding='utf-8', dst_encoding='utf-8-sig')


if __name__ == '__main__':
    show_file_dialog()
