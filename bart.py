import urllib2
import xmltodict
import serial
import time
import collections
import os
import glob
import sys

class BartClock:
    api_key = os.environ.get('BART_API_KEY')
    station = os.environ.get('BART_STATION', 'plza')
    train_direction = os.environ.get('BART_DIRECTION', 'South')
    device_name = ''
    # data = {}

    intervals = {
        'no_device': 5,
        'no_bart': 60,
        'server_poll': 12,
        'serial_write': 1,
        'serial_error': 5,
        'load_error': 10
    }

    def get_device(self):
        device_names = glob.glob('/dev/tty.usbmodem*')
        has_device = len(device_names) > 0
        if has_device:
            self.device_name = device_names[0]
        return has_device

    def real_thing(self):
        try:
            hasDevice = self.get_device();

            if not hasDevice:
                print 'arduino device not found'
                time.sleep(self.intervals['no_device'])
                return


            file = urllib2.urlopen("http://api.bart.gov/api/etd.aspx?cmd=etd&orig=" + self.station + "&key=" + self.api_key, timeout = 5)
            data = file.read()
            file.close()

            data = xmltodict.parse(data)

        except:
            print 'error loading data'
            time.sleep(intervals['load_error'])
            return

        minutes = []
        etds = data['root']['station']['etd']

        if type(etds) is collections.OrderedDict:
            etds = [etds]

        for etd in etds:
            estimates = etd['estimate']
            if type(estimates) is collections.OrderedDict:
                estimates = [estimates]
            for estimate in estimates:
                if estimate['direction'] == self.train_direction:
                    if estimate['minutes'] == 'Arriving':
                        minutes.append(1)
                    elif estimate['minutes'] != 'Leaving':
                        minutes.append(int(estimate['minutes']))

        if len(minutes) == 0:
            time.sleep(intervals['no_bart'])
            return

        minutes.sort()

        minute_string = ''
        for minute in minutes:
            minute_string += str(minute) + ' '
        minute_string += ';'

        smallInterval = float(self.intervals['serial_write'])
        largeInterval = self.intervals['server_poll']
        printl(minute_string + ' ')
        for i in range(0, int(largeInterval/smallInterval)):
            try:
                ser = serial.Serial(self.device_name, 9600)
                ser.write(minute_string)
                ser.close()
                printl('.')
                time.sleep(smallInterval)
            except Exception as e:
                print e
                print 'error with serial communication'
                time.sleep(self.intervals['serial_error'])
                return
        sys.stdout.write('\n')


    def fake(self):
        try:
            get_device();
            ser = serial.Serial(self.device_name, 9600)
            faux_data = '4 11 23 ;'
            ser.write(faux_data)
            ser.close()
        except:
            pass
        finally:
            time.sleep(15)

    def __init__(self):
        while True:
            self.real_thing()
            # fake()

def printl(str):
    sys.stdout.write(str)
    sys.stdout.flush()

if __name__ == '__main__':
    bart = BartClock()
