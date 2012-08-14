<!DOCTYPE html>
<html lang="pl">

<body style="height: 100%;">
<div align="center" class="title">
Zapotrzebowanie żywnościowe (Formularz 11A)
</div>
<table class="header" width="100%;" border="0">
<tr>
<td width="50%">Związek Harcerstwa Polskiego</td>
<td width="50%" align="right">Obóz w: @PLACE@</td>
</tr>
<tr>
<td width="50%">Data: @DATE@</td>
<td width="50%" align="right">Organizator: @ORG@</td><td />
</tr></table>

<table class="header"><tr>
<td>Zatwierdzona w planie żywnościowym średnia stawka dzienna: @AVGCOSTS@ zł</td>
</tr></table>

<br />

<table id="meals" class="maincont" width="100%" border="1">
<thead><tr>
	@TABLE_HEADERS@
</tr></thead>
<tbody>
<tr>@TABLE_CONTENTS@</tr>
</tbody>
<tfoot>
</tfoot>
</table>
<!--
<p>
Posiłki dodatkowe - wyszczególnienie:<br />
@TABLE_CONTENT_ADD@
</p>
-->
<p>Dane o stanie żywieniowym (osoboposiłki - liczba na wszystkich posiłkach):<br />
1. Liczba posiłków dla uczestników obozu: @SCOUTSNO@<br />
2. Liczba posiłków dla kadry obozowej: @LEADERSNO@<br />
3. Liczba posiłków dla pozostałych: @OTHERSNO@<br />
Wydanych posiłków razem: @ALL@
</p>
<p>Przeciętny koszt wyżywienia jednej osoby: @AVG@ zł</p>
<br />
<table class="maincont" width="100%" border="0">
<thead><tr>
	<td>Sporządził</td>
	<td>Sprawdził</td>
	<td>Zatwierdził</td>
</tr></thead>
<tbody>
<tr><td height="100px">
<br />
<br />
</td><td>
</td><td>
</td></tr>
<tr><td>
@QUATER@
</td><td>
@OTHER@
</td><td>
@LEADER@
</td></tr>
<tr><td>
Kwatermistrz lub zaopatrzeniowiec
</td><td>
Przestawiciel rady obozu
</td><td>
Komendant obozu
</td></tr>
</tbody>
<tfoot>
</tfoot>
</table>
</body>
</html>