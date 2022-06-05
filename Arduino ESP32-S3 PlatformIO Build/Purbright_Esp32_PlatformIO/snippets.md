# HTML Snippets

```html
<div class="container">
    <div class="row">
        <div class="col-md-12">
            <h1>Hello World</h1>
        </div>
    </div>
</div>
```

## Calculate the range of Wifi signal strength

´´´txt
Signal transmission in ft^2 = ((dBm + 4,000) - 2,000) / 42.7

4,000 = nominal base dB of average consumer grade WIFI antenna
2,000 = Nominal base dB of peak signal interference
42.7 = a constant for conversion to square feet
´´´

```cpp
int signal_range = ((dBm + 4,000) - 2,000) / 42.7;

WiFi.setOutputPower(dBm);
From the point of view of the client:
a: How low is to low - when is the client not able to see the server
b: How close to this lowest point are we willing to go?


1. Client tells the server when it needs a stronger signal - do this in levels
2. Server lowers signal by a certain value each time frame 
3. Every x amount of time based on how long device can be without connection - it will pulse the signal at max sending out a wake up call to devices that have potentially lost connection
```

```cpp
if (!PRODUCTION) {
  // do development stuff
}
```

## Useful Links

[Wifi QR code generator](https://qifi.org/)
