import argparse
import socket


parser = argparse.ArgumentParser(description='Send wc6/wc7 files to IP.')
parser.add_argument('ip', help='Console IP address.')
parser.add_argument('filenames', nargs='+', help='wc6/wc7 files.')


def sendwcx(ip, filename):
    s = socket.socket()
    try:
        s.connect((ip, 9000))
        with open(filename, 'rb') as f:
            l = b"PKSMOTA" + f.read(264)
            while l != b'PKSMOTA':
                s.send(l)
                l = b"PKSMOTA" + f.read(264)
        s.close()
    except Exception as e:
        s.close()  # close this socket at all cost
        raise e


def main(args):
    for filename in args.filenames:
        sendwcx(args.ip, filename)


if __name__ == '__main__':
    main(parser.parse_args())
