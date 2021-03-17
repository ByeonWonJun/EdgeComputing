$(function () {

    kakaomap();

    function kakaomap() {

        <script type="text/javascript" src="//dapi.kakao.com/v2/maps/sdk.js?appkey=90f20e745093a3389f1b1d00eb783ea1"></script>

        var container = document.getElementById('map'); //지도를 담을 영역의 DOM 레퍼런스
        var options = { //지도를 생성할 때 필요한 기본 옵션
	        center: new kakao.maps.LatLng(33.450701, 126.570667), //지도의 중심좌표.
	        level: 3 //지도의 레벨(확대, 축소 정도)
        };

        var map = new kakao.maps.Map(container, options); //지도 생성 및 객체 리턴

    }


})