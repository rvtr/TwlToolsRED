<?xml version="1.0" encoding="UTF-8" ?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:template match="/">
	<html>
	<head>
		<title>TWL Rom Information List</title>
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
			  text-decoration:underline;
			  text-underline:single;
			  margin-top: 24px;
			  margin-bottom: 10px;
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
			div.path
			{
			  font-size: 12px;
			}
			span.num-error
			{
			  color: #FF0000;
			  font-size: 12px;
			  font-weight: bold;
			}
			span.num-normal
			{
			  font-size: 12px;
			  font-weight: bold;
			}
			div.error
			{
			  color: #FF0000;
			}
			hr {
			  width: 1000px;
			  margin-left: 0;
			}
			tr {
			  text-align: left;
			}
			tr.even {
			  background-color: #FFFFFF;
			}
			tr.odd {
			  background-color: #D6D3CE;
			}
	        th {
	          font-size: 12px;
			  background-color: #000000;
			  color: #dddddd;
			  padding-top: 2px;
			  padding-bottom: 2px;
	        }
			th.label {
			  width: 400px;
			}
			th.value {
			  width: 300px;
			}
			th.type {
			  width: 300px;
			}
			th.version {
			  width: 400px;
			}
			th.is-static {
			  width: 300px;
			}
			th.publisher {
			  width: 200px;
			}
			th.name {
			  width: 400px;
			}
			th.note {
			  width: 400px;
			}
			th.errname {
			  width: 200px;
			}
			th.errtype {
			  width: 200px;
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
			  padding-top: 2px;
			  padding-bottom: 2px;
	        }
			td.label {
			  width: 400px;
			}
			td.value {
			  width: 300px;
			}
			td.sp-value {
			  width: 300px;
			  color: #0000FF;
			}
			td.type {
			  width: 300px;
			}
			td.version {
			  width: 400px;
			}
			td.is-static {
			  width: 300px;
			}
			td.publisher {
			  width: 200px;
			}
			td.name {
			  width: 400px;
			}
			td.note {
			  width: 400px;
			}
			td.errname {
			  width: 200px;
			}
			td.errtype {
			  width: 200px;
			}
			td.errrange {
			  width: 100px;
			}
			td.errmsg {
			  width: 600px;
			}
		</style>
	</head>

	<body>

	<xsl:apply-templates />

	</body>
	</html>
</xsl:template>

<!-- Root -->

<xsl:template match="twl-master-editor">
	<xsl:apply-templates select="game" />
	<xsl:apply-templates select="error-num-info" />
	<div class="path">
		<xsl:value-of select="path" />
	</div>
	<hr />
	<xsl:apply-templates select="basic-rom-info" />
	<hr />
	<xsl:apply-templates select="twl-extended-info" />
	<hr />
	<xsl:apply-templates select="access-control-info" />
	<hr />
	<xsl:apply-templates select="dsi-ware-info" />
	<hr />
	<xsl:apply-templates select="market-and-rating-info" />
	<hr />
	<xsl:apply-templates select="sdk-version-info" />
	<hr />
	<xsl:apply-templates select="middleware-info" />
	<hr />
	<xsl:apply-templates select="error-info" />
	<hr />
	<xsl:apply-templates select="warning-info" />
	<hr />
	<div class="footer">
		generated on <xsl:apply-templates select="time-info" /> by MasterEditorTWL ver. <xsl:value-of select="@version" />
	</div>
</xsl:template>

<!-- Title Info -->

<xsl:template match="game">
	<div class="game-title">
		<xsl:value-of select="title-name" />(<xsl:value-of select="game-code" />)
	</div>
	<div class="game-version">
		version: <xsl:value-of select="rom-version" />
	</div>
</xsl:template>

<!-- Each Section -->

<xsl:template match="basic-rom-info">

	<div class="index"><xsl:value-of select="index" /></div>

	<xsl:apply-templates select="info-list" />
</xsl:template>

<xsl:template match="twl-extended-info">

	<div class="index"><xsl:value-of select="index" /></div>

	<xsl:apply-templates select="info-list" />
</xsl:template>

<xsl:template match="access-control-info">

	<div class="index"><xsl:value-of select="index" /></div>

	<xsl:apply-templates select="info-list" />
</xsl:template>

<xsl:template match="dsi-ware-info">

	<div class="index"><xsl:value-of select="index" /></div>

	<xsl:apply-templates select="info-list" />
</xsl:template>

<xsl:template match="market-and-rating-info">

	<div class="index"><xsl:value-of select="index" /></div>

	<xsl:apply-templates select="info-list" />
</xsl:template>

<xsl:template match="sdk-version-info">

	<div class="index"><xsl:value-of select="index" /></div>

	<xsl:apply-templates select="sdk-version-list" />
</xsl:template>

<xsl:template match="middleware-info">

	<div class="index"><xsl:value-of select="index" /></div>

	<xsl:apply-templates select="middleware-list" />
</xsl:template>

<xsl:template match="error-info">

	<div class="index"><xsl:value-of select="index" /></div>

	<xsl:apply-templates select="error-list" />
	<hr />
	<xsl:apply-templates select="error-caption-list" />
</xsl:template>

<xsl:template match="warning-info">

	<div class="index"><xsl:value-of select="index" /></div>

	<xsl:apply-templates select="warning-list" />
	<hr />
	<xsl:apply-templates select="error-caption-list" />
</xsl:template>

<xsl:template match="error-num-info">
	<xsl:choose>
	<xsl:when test="@error-num &gt; 0">
		<span class="num-error">
			<xsl:value-of select="@error-title" /> : <xsl:value-of select="@error-num" />
		</span>
	</xsl:when>
	<xsl:otherwise>
		<span class="num-normal">
			<xsl:value-of select="@error-title" /> : <xsl:value-of select="@error-num" />
		</span>
	</xsl:otherwise>
	</xsl:choose>
	,
	<xsl:choose>
	<xsl:when test="@warning-num &gt; 0">
		<span class="num-error">
			<xsl:value-of select="@warning-title" /> : <xsl:value-of select="@warning-num" />
		</span>
	</xsl:when>
	<xsl:otherwise>
		<span class="num-normal">
			<xsl:value-of select="@warning-title" /> : <xsl:value-of select="@warning-num" />
		</span>
	</xsl:otherwise>
	</xsl:choose>
</xsl:template>

<xsl:template match="time-info">
	<xsl:value-of select="@month" />&#160;<xsl:value-of select="@day" />, <xsl:value-of select="@year" />&#160;<xsl:value-of select="@time" />
</xsl:template>

<!-- Make tables from Lists -->

<xsl:template match="info-list">
	<table cellspacing="0">
		<tr>
			<th class="label">Item Name</th>
			<th class="value">Value</th>
			<th class="type">Note</th>
		</tr>

		<xsl:apply-templates />

	</table>
</xsl:template>

<xsl:template match="info">
<xsl:if test="(@num mod 2)=1">
	<tr class="odd">
		<td class="name"><xsl:value-of select="label" /></td>
		<td class="value"><xsl:value-of select="value" /></td>
		<td class="type"><xsl:value-of select="type" /></td>
	</tr>
</xsl:if>
<xsl:if test="(@num mod 2)=0">
	<tr class="even">
		<td class="name"><xsl:value-of select="label" /></td>
		<td class="value"><xsl:value-of select="value" /></td>
		<td class="type"><xsl:value-of select="type" /></td>
	</tr>
</xsl:if>
</xsl:template>

<xsl:template match="sp-info">
<xsl:if test="(@num mod 2)=1">
	<tr class="odd">
		<td class="name"><xsl:value-of select="label" /></td>
		<td class="sp-value"><xsl:value-of select="value" /></td>
		<td class="type"><xsl:value-of select="type" /></td>
	</tr>
</xsl:if>
<xsl:if test="(@num mod 2)=0">
	<tr class="even">
		<td class="name"><xsl:value-of select="label" /></td>
		<td class="sp-value"><xsl:value-of select="value" /></td>
		<td class="type"><xsl:value-of select="type" /></td>
	</tr>
</xsl:if>
</xsl:template>

<!-- Make the SDK version table -->

<xsl:template match="sdk-version-list">
	<table cellspacing="0">
		<tr>
			<th class="version">Version</th>
			<th class="is-static">Type</th>
			<th class="type">Note</th>
		</tr>

		<xsl:apply-templates />

	</table>
</xsl:template>

<xsl:template match="sdk-version">
<xsl:if test="(@num mod 2)=1">
	<tr class="odd">
		<td class="version"><xsl:value-of select="version" /></td>
		<td class="is-static"><xsl:value-of select="is-static" /></td>
		<td class="type"></td>
	</tr>
</xsl:if>
<xsl:if test="(@num mod 2)=0">
	<tr class="even">
		<td class="version"><xsl:value-of select="version" /></td>
		<td class="is-static"><xsl:value-of select="is-static" /></td>
		<td class="type"></td>
	</tr>
</xsl:if>
</xsl:template>

<!-- Make the Libraries table -->

<xsl:template match="middleware-list">
	<table cellspacing="0">
		<tr>
			<th class="publisher">Publisher</th>
			<th class="name">Name</th>
			<th class="note">Note</th>
		</tr>

		<xsl:apply-templates />

	</table>
</xsl:template>

<xsl:template match="middleware">
	<xsl:if test="(@num mod 2)=1">
	<tr class="odd">
		<td class="publisher"><xsl:value-of select="publisher" /></td>
		<td class="name"><xsl:value-of select="name" /></td>
		<td class="note"><xsl:value-of select="note" /></td>
	</tr>
	</xsl:if>
	<xsl:if test="(@num mod 2)=0">
	<tr class="even">
		<td class="publisher"><xsl:value-of select="publisher" /></td>
		<td class="name"><xsl:value-of select="name" /></td>
		<td class="note"><xsl:value-of select="note" /></td>
	</tr>
	</xsl:if>
</xsl:template>

<!-- Make the Error and Warning table -->

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
	<xsl:if test="(@num mod 2)=1">
	<tr class="odd">
		<td class="errname"><div class="error"><xsl:value-of select="errname" /></div></td>
		<td class="errtype"><div class="error"><xsl:value-of select="errtype" /></div></td>
		<td class="errmsg"><div class="error"><xsl:value-of select="errmsg" /></div></td>
	</tr>
	</xsl:if>
	<xsl:if test="(@num mod 2)=0">
	<tr class="even">
		<td class="errname"><div class="error"><xsl:value-of select="errname" /></div></td>
		<td class="errtype"><div class="error"><xsl:value-of select="errtype" /></div></td>
		<td class="errmsg"><div class="error"><xsl:value-of select="errmsg" /></div></td>
	</tr>
	</xsl:if>
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
	<xsl:if test="(@num mod 2)=1">
	<tr class="odd">
		<td class="errname"><div class="error"><xsl:value-of select="errname" /></div></td>
		<td class="errtype"><div class="error"><xsl:value-of select="errtype" /></div></td>
		<td class="errmsg"><div class="error"><xsl:value-of select="errmsg" /></div></td>
	</tr>
	</xsl:if>
	<xsl:if test="(@num mod 2)=0">
	<tr class="even">
		<td class="errname"><div class="error"><xsl:value-of select="errname" /></div></td>
		<td class="errtype"><div class="error"><xsl:value-of select="errtype" /></div></td>
		<td class="errmsg"><div class="error"><xsl:value-of select="errmsg" /></div></td>
	</tr>
	</xsl:if>
</xsl:template>

<xsl:template match="error-caption-list">
	<xsl:apply-templates />
</xsl:template>

<xsl:template match="error-caption">
	<div class="caption"><xsl:value-of select="text"/></div>
</xsl:template>

</xsl:stylesheet> 
