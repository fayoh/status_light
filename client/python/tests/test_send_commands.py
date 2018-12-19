import statuslight_client.statuslight as statuslight
from statuslight_client.statuslight import Statuslight

import pytest
from unittest.mock import call


@pytest.fixture(autouse=True)
def mock_serial(mocker):
    mocker.patch.object(statuslight.serial, 'Serial')


def test_default_values(mocker):
    statuslight.serial.Serial.assert_not_called()
    Statuslight()
    statuslight.serial.Serial.assert_called_once()
    # TODO: have a look at the actual values


def test_intensity(mocker):
    obj = statuslight.Statuslight()
    obj.set_intensity(statuslight.RED, 0x1)
    obj.conn.write.assert_called_once_with(b'!\x01')


def test_clear(mocker):
    obj = statuslight.Statuslight()
    obj.clear()
    calls = [call(b'a\x00'), call(b'A\x00'), call(b'!\x00'),
	     call(b'b\x00'), call(b'B\x00'), call(b'"\x00'),
	     call(b'd\x00'), call(b'D\x00'), call(b'$\x00')]
    assert obj.conn.write.call_count == len(calls)
    obj.conn.write.assert_has_calls(calls)


def test_blink(mocker):
    obj = statuslight.Statuslight()
    obj.set_blink(statuslight.GREEN, 0x00)
    obj.conn.write.assert_called_once_with(b'D\x00')


def test_pulse(mocker):
    obj = statuslight.Statuslight()
    obj.set_pulse(statuslight.GREEN, 0x02)
    obj.conn.write.assert_called_once_with(b'd\x02')
