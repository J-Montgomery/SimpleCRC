import requests
import sys
import argparse
from collections import namedtuple
from bs4 import BeautifulSoup

CrcDef = namedtuple('CRC', ['name', 'width', 'poly',
                            'refin', 'refout', 'init',
                            'xorout', 'check', 'residue'])

def get_db_content(url):
    r = requests.get(url)
    return r.text

def crc_normalize(str):
    return " ".join(str.split()).replace('\"', '')

def get_crc_defs(content):
    parser = BeautifulSoup(content, 'html.parser')
    pars = parser.find_all("p", class_=["academic","attested","confirmed","third-party"], text=True)
    crc_defs = [code.find("code").text for code in pars]
    crcs = [CrcDef(**dict(x.split("=")
                    for x in crc_normalize(definition).split(" ")))
                for definition in crc_defs]
    crcs = [x._replace(name = x.name.split('/')[1]) for x in crcs]
    return crcs

def write_file(fp, definitions):
    def_format = \
    '''{name} {width} {poly}''' \
    ''' {refin} {refout} {init}''' \
    ''' {xorout} {check} {residue};\n'''

    for crc in definitions:
        string_def = def_format.format(**crc._asdict())
        fp.write(string_def)
    return

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-o", help="Output file", required=True, dest='db_name')
    parser.add_argument("-u", "--url", help="CRC database to scrape", required=True)
    args = parser.parse_args()

    with open(args.db_name, 'w') as f:
        content = get_db_content(args.url)
        defs = get_crc_defs(content)
        write_file(f, defs)

