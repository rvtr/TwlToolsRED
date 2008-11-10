<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:ss="urn:schemas-microsoft-com:office:spreadsheet">
<xsl:output method="text" encoding="UTF-8" />

<xsl:template match="/">
	<xsl:apply-templates select="ss:Workbook/ss:Worksheet/ss:Table/ss:Row/ss:Cell" />
</xsl:template>

<xsl:template match="*">
	<xsl:apply-templates />
</xsl:template>

<!-- 提出バージョンのセルだけ抜き出す -->
<xsl:template match="ss:Cell">
	<!-- NamedCellノードの名前を変数に代入 -->
	<xsl:variable name="tag">
		<xsl:apply-templates select="ss:NamedCell" />
	</xsl:variable>
	<!-- NamedCellノードの名前がSubmitVersionのときのみ出力 -->
	<xsl:if test="$tag='SubmitVersion'">
		<xsl:value-of select="ss:Data" />
	</xsl:if>
</xsl:template>

<!-- NamedCellノードの名前(属性で指定されている)を取得 -->
<xsl:template match="ss:NamedCell">
	<xsl:value-of select="@ss:Name" />
</xsl:template>

</xsl:stylesheet> 
