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
	<!-- NamedCellノードの名前がRegionのときのみ出力 -->
	<xsl:if test="$tag='Region'">
		<Region><xsl:value-of select="ss:Data" /></Region>
	</xsl:if>
	<xsl:if test="$tag='RatingCERO'">
		<RatingCERO><xsl:value-of select="ss:Data" /></RatingCERO>
	</xsl:if>
	<xsl:if test="$tag='RatingESRB'">
		<RatingESRB><xsl:value-of select="ss:Data" /></RatingESRB>
	</xsl:if>
	<xsl:if test="$tag='RatingUSK'">
		<RatingUSK><xsl:value-of select="ss:Data" /></RatingUSK>
	</xsl:if>
	<xsl:if test="$tag='RatingPEGI'">
		<RatingPEGI><xsl:value-of select="ss:Data" /></RatingPEGI>
	</xsl:if>
	<xsl:if test="$tag='RatingPEGIPRT'">
		<RatingPEGIPRT><xsl:value-of select="ss:Data" /></RatingPEGIPRT>
	</xsl:if>
	<xsl:if test="$tag='RatingPEGIBBFC'">
		<RatingPEGIBBFC><xsl:value-of select="ss:Data" /></RatingPEGIBBFC>
	</xsl:if>
	<xsl:if test="$tag='RatingOFLC'">
		<RatingOFLC><xsl:value-of select="ss:Data" /></RatingOFLC>
	</xsl:if>
	<xsl:if test="$tag='IsUnnecessaryRating'">
		<IsUnnecessaryRating><xsl:value-of select="ss:Data" /></IsUnnecessaryRating>
	</xsl:if>
</xsl:template>

<!-- NamedCellノードの名前(属性で指定されている)を取得 -->
<xsl:template match="ss:NamedCell">
	<xsl:value-of select="@ss:Name" />
</xsl:template>

</xsl:stylesheet> 
