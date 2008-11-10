<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:ss="urn:schemas-microsoft-com:office:spreadsheet">

<xsl:template match="/">
	<Sheet>
	<xsl:apply-templates select="ss:Workbook/ss:Worksheet/ss:Table/ss:Row/ss:Cell" />
	</Sheet>
</xsl:template>

<xsl:template match="*">
	<xsl:apply-templates />
</xsl:template>

<!-- 特定のセルだけ抜き出して新たにタグづけする -->
<xsl:template match="ss:Cell">
	<!-- NamedCellノードの名前を変数に代入 -->
	<xsl:variable name="tag">
		<xsl:apply-templates select="ss:NamedCell" />
	</xsl:variable>
	<!-- NamedCellノードの名前がSubmitVersionのときのみ出力 -->
	<xsl:if test="$tag='SubmitVersion'">
		<SubmitVersion><xsl:value-of select="ss:Data" /></SubmitVersion>
	</xsl:if>
	<xsl:if test="$tag='GameCode'">
		<GameCode><xsl:value-of select="ss:Data" /></GameCode>
	</xsl:if>
	<xsl:if test="$tag='RomVersion'">
		<RomVersion><xsl:value-of select="ss:Data" /></RomVersion>
	</xsl:if>
	<xsl:if test="$tag='CRC'">
		<CRC><xsl:value-of select="ss:Data" /></CRC>
	</xsl:if>
</xsl:template>

<!-- NamedCellノードの名前(属性で指定されている)を取得 -->
<xsl:template match="ss:NamedCell">
	<xsl:value-of select="@ss:Name" />
</xsl:template>

</xsl:stylesheet> 
