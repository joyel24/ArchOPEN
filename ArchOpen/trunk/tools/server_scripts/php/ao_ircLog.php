<html>
<body>
<H1>Irc log for #archopen</h1>

<?

class mirctext {
    var $colors;
    function mirctext() {
        $this->colors = array(
            '#FFFFFF',              //0
            '#000000',              //1
            '#000080',              //2
            '#00AA00',              //3
            '#FF0000',              //4
            '#AA0000',              //5
            '#AA00AA',              //6
            '#FF8040',              //7
            '#FFFF00',              //8
            '#00FF00',              //9
            '#008080',              //10
            '#00FFFF',              //11
            '#0000FF',              //12
            '#FF00FF',              //13
            '#808080',              //14
            '#C0C0C0');             //15
    }

    function returncolorstyle ($fg,$bg) {
        $tmp = 'color: '.$this->colors[intval($fg)];
        if ($bg != "")
            $tmp .= ' ; background: '.$this->colors[intval($bg)].';';
        return $tmp;
    }
    function parseunderline($text) {
        return  preg_replace("/\37(.*?)(\37|$)/",'<u>\\1</u>',$text);
    }
    function parsebold($text) {
        return  preg_replace("/\2(.*?)(\2|$)/",'<b>\\1</b>',$text);
    }
    function parselink($text) {
        return preg_replace("/(http:\/\/[\w.:\-\/#\?=\~]{2,})\b/", '<a href="\\1">\\1</a>', $text);
    }
    function parsecolors($text) {
        $tmp = preg_replace_callback("/(\3(\d{1,2}))(,(\d{1,2}))?([^\3]*)/",array($this,'callkhaled'),$text);
        return str_replace("\3",'',$tmp);
    }
    
    function callkhaled($svar) {
    return "<span style=\"".$this->returncolorstyle($svar[2],$svar[4])."\">".$svar[5]."</span>";
    }
    
}

function mirc2html($mirc) {
    $uncode = new mirctext;
    $mirc = htmlentities($mirc);
    // gestion du souligné
    $mirc = $uncode->parseunderline($mirc);
    // gestion du gras
    $mirc = $uncode->parsebold($mirc);
    // gestion des couleurs
    $mirc = $uncode->parsecolors($mirc);
    //gestion des liens
    $mirc = $uncode->parselink($mirc);
    // retour
    return $mirc;
}


if (isset($_GET["disp_log"]))
{
$disp_log=$_GET["disp_log"];
echo "<a href='ao_ircLog.php'>Back</a><br><br>\n";
     $string_text=file_get_contents("irclog/".$disp_log); // load text file in var
     /*$userdatei = fopen("irclog/".$disp_log,"r");

                   while(!feof($userdatei))
                   {
                      $zeile = fgets($userdatei,10024);
                      echo mirc2html($zeile); //i didn't change the mirc2html-function
                      print "<br />";
                   }


                   print "</div></div>";
            fclose($userdatei);*/

     $new_text=htmlspecialchars($string_text);
	$new_text=nl2br($new_text); // convert CR & LF in <br> in newvar
     echo $new_text; // print out HTML formatted text
    
     unset($string_text, $new_text); // clear all vars to unload memory

echo "<br><br><a href='ao_ircLog.php'>Back</a><br><br>\n";
}
else
{

echo "<a href='http://www.archopen.org/tiki-index.php'>back</a><p>\n";

$time_inc=time();

$first_day=mktime(0,0,0,8,01,06);

/* creating table of date + including files and mark presence*/
$prevInc=$time_inc-24*60*60;
$nxtYear=1;
$val=getdate($time_inc);
echo "<p><b>".$val["year"]."</b><br><table>";
echo "<tr><td width=100><i>". $val["month"]."</i>:</td>";

while($time_inc>=$first_day)
{
    $fname="archopen." . date("Ymd",$time_inc). ".log";
    $val=getdate($time_inc);
    $prevVal=getdate($prevInc);
    //echo $val["mday"]."<br>";
    if($val["mon"]==1)
        $nxtYear=1;
     if($val["mon"]==12 && $nxtYear==1)
    {
        $nxtYear=0;
        echo "</tr></table><p><b>".$val["year"]."</b><br><table>";
    }
    if($prevVal["mday"]==1)
    {
        if($val["mon"]!=12)
            echo "</tr>";
        echo "<tr><td width=100><i>". $val["month"]."</i>:</td>";
    }
    if(file_exists("irclog/".$fname))
        echo "<td width=10><center><a href='ao_ircLog.php?disp_log=".$fname."'>".$val["mday"]."</center></a></td>";
    else
        echo "<td width=10><center>".$val["mday"]."</center></td>";
    //echo date("Ymd",$time_inc) . " ";
    /*if(file_exists("irclog/".$fname))
        echo "<a href='ao_ircLog.php?disp_log=".$fname."'>".date("Ymd",$time_inc)."</a><br>";    
        */
        
    $prevInc=$time_inc;
    $time_inc-=24*60*60;
}
echo "</tr></table><p><a href='http://www.archopen.org/tiki-index.php'>back</a>\n";
}
?>

</body>
</html>