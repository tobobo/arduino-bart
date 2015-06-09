import urllib2
import xmltodict
import serial
import time
import collections
import os

api_key = os.environ.get('BART_API_KEY')
station = os.environ.get('BART_STATION', 'plza')
train_direction = os.environ.get('BART_DIRECTION', 'South')

def realThing():
    try:
        file = urllib2.urlopen("http://api.bart.gov/api/etd.aspx?cmd=etd&orig=" + station + "&key=" + api_key)
        data = file.read()
        file.close()

        data = xmltodict.parse(data)

        minutes = []
        etds = data['root']['station']['etd']

        if type(etds) is collections.OrderedDict:
            etds = [etds]
        for etd in etds:
            estimates = etd['estimate']
            if type(estimates) is collections.OrderedDict:
                estimates = [estimates]
            for estimate in etd['estimate']:
                if estimate['direction'] == train_direction:
                    if estimate['minutes'] not in ['Arriving', 'Leaving']:
                        minutes.append(int(estimate['minutes']))

        minutes.sort()

        minute_string = ''
        for minute in minutes:
            minute_string += str(minute) + ' '
        minute_string += ';'

        print minute_string
        try:
            ser = serial.Serial('/dev/tty.usbmodem1411', 9600)
            ser.write(minute_string)
            ser.close()
        except:
            pass
        finally:
            time.sleep(15)

    except:
        print 'error loading data'
        time.sleep(15)

def fake():
    try:
        ser = serial.Serial('/dev/tty.usbmodem1411', 9600)
        faux_data = '4 11 23 ;'
        ser.write(faux_data)
        print faux_data;
        ser.close()
    except:
        pass
    finally:
        time.sleep(15)

while True:
    realThing()
    # fake()
