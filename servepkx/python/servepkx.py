import argparse
import socket


parser = argparse.ArgumentParser(description='Send pk6/pk7 files to IP.')
parser.add_argument('ip', help='Console IP address.')
parser.add_argument('filenames', nargs='+', help='pk6/pk7 files.')


def sendpkx(ip, filename):
    s = socket.socket()
    try:
        s.connect((ip, 9000))
        with open(filename, 'rb') as f:
            l = f.read(232)
            while l != '':
                s.send(l)
                l = f.read(232)
        s.close()
    except Exception as e:
        s.close()  # close this socket at all cost
        raise e


def main(args):
    for filename in args.filenames:
        sendpkx(args.ip, filename)


if __name__ == '__main__':
    main(parser.parse_args())
