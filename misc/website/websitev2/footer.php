<div class="center">
    <img src="/images/sflogo.png" width="120" height="30" alt="SourceForge logo" title="Process Hacker is hosted by SourceForge.net">
    <br>
    Copyright &copy; 2008-2012 wj32
</div>

<?php
if (@$includejs) {
    echo
    "<script type=\"text/javascript\" src=\"/js/lytebox.min.js\"></script>";
    if ($pagetitle == "Overview") {
        echo
        "<script type=\"text/javascript\" src=\"http://s7.addthis.com/js/250/addthis_widget.js#pubid=dmex\"></script>";
    }
}
?>

<!-- Google Analytics (Async)-->
<script type="text/javascript">
    var _gaq = _gaq || [];
    _gaq.push(['_setAccount', 'UA-22023876-1']);
    _gaq.push(['_trackPageview']);
    (function() {
        var ga = document.createElement('script');
        ga.type = 'text/javascript';
        ga.async = true;
        ga.src = ('https:' == document.location.protocol ? 'https://ssl' : 'http://www') + '.google-analytics.com/ga.js';
        var s = document.getElementsByTagName('script')[0];
        s.parentNode.insertBefore(ga, s);
    })();
</script>
<!-- End Google Analytics -->
</body>
</html>