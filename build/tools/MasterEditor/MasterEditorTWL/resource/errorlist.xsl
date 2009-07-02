<?xml version="1.0" encoding="UTF-8" ?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:template match="/">
	<html>
	<head>
		<title>TWL Error List</title>
		<style type="text/css">
			body {
			  font-family: "verdana";
			  font-size: 12px;
			  text-align: left;
			}
			div.game-title {
			  font-size: 16px;
			  font-weight: bold;
			}
			div.game-version {
			  font-size: 12px;
			  font-weight: bold;
			}
			div.index
			{
			  font-size: 14px;
			  font-weight: bold;
			}
			div.spacer
			{
			  font-size: 14px;
			}
			div.caption
			{
			  font-size: 10px;
			  font-weight: bold;
			}
			hr {
			  width: 1000px;
			  margin-left: 0;
			}
			tr {
			  text-align: left;
			}
	        th {
	          font-size: 12px;
			  background-color: #D6D3CE;
			  color: black;
	        }
			th.errname {
			  width: 200px;
			}
			th.errtype {
			  width: 100px;
			}
			th.errrange {
			  width: 100px;
			}
			th.errmsg {
			  width: 600px;
			}
	        td {
	          font-weight: normal;
	          font-size: 12px;
	        }
		</style>
	</head>

	<body>

	<xsl:apply-templates />

	</body>
	</html>
</xsl:template>

<xsl:template match="twl-master-editor">
	<xsl:apply-templates select="game" />
	<hr />
	<div class="index">Error</div>
	<xsl:apply-templates select="error-list" />
	<hr />
	<div class="index">Warning</div>
	<xsl:apply-templates select="warning-list" />
	<hr />
	<xsl:apply-templates select="error-caption-list" />
	<hr />
	<div class="footer">
		generated by MasterEditorTWL ver. <xsl:value-of select="@version" />
	</div>
</xsl:template>

<xsl:template match="game">
	<div class="game-title">
		<xsl:value-of select="title-name" />(<xsl:value-of select="game-code" />)
	</div>
	<div class="game-version">
		version: <xsl:value-of select="rom-version" />
	</div>
</xsl:template>

<xsl:template match="error-list">
	<table cellspacing="0">
		<tr>
			<th class="errname">Item Name</th>
			<th class="errtype">Type</th>
			<th class="errmsg">Reason</th>
		</tr>

		<xsl:apply-templates />

	</table>
</xsl:template>

<xsl:template match="error">
	<tr>
		<td class="errname"><xsl:value-of select="errname" /></td>
		<td class="errtype"><xsl:value-of select="errtype" /></td>
		<td class="errmsg"><xsl:value-of select="errmsg" /></td>
	</tr>
</xsl:template>

<xsl:template match="warning-list">
	<table cellspacing="0">
		<tr>
			<th class="errname">Item Name</th>
			<th class="errtype">Type</th>
			<th class="errmsg">Reason</th>
		</tr>

		<xsl:apply-templates />

	</table>
</xsl:template>

<xsl:template match="warning">
	<tr>
		<td class="errname"><xsl:value-of select="errname" /></td>
		<td class="errtype"><xsl:value-of select="errtype" /></td>
		<td class="errmsg"><xsl:value-of select="errmsg" /></td>
	</tr>
</xsl:template>

<xsl:template match="error-caption-list">
	<xsl:apply-templates />
</xsl:template>

<xsl:template match="error-caption">
	<div class="caption"><xsl:value-of select="text"/></div>
</xsl:template>

</xsl:stylesheet> 
