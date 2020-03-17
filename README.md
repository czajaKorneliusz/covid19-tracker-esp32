<h1 align="center">Covid-19 tracker for ESP32</h1>
<hr>
<br>
<h2>Description</h2>
<p>Project reads data from Web Page <a href="https://www.worldometers.info/coronavirus/">https://www.worldometers.info/coronavirus/</a> and displays current infected count.</p>
<br>
<p>By default it was created for <a href="https://github.com/Xinyuan-LilyGO/TTGO-T-Display">TTGO-T-Display</a>, but it is possible to convert to other device.</p>
<p>Data is refreshed every 10 minutes (60 000 000µs).</p>
<p>User can also display current count anytime by using button to wake the device from deep sleep.</p>
<p>ALERT_PIN is used to send visual or audio signal when the count has changed.</p>