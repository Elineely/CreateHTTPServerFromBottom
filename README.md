# 🌏HTTP 서버 구현 프로젝트 / cuteWebServ
<img width="890" alt="스크린샷 2023-12-19 오후 2 44 32" src="https://github.com/Elineely/CreateWebServerFromBottom/assets/80635378/b326d590-1ef0-4542-9ddf-568ff64bb23d">

cuteWebserv는 C++로 작성된 간단한 HTTP 서버입니다. 이 서버는 NGINX와 같은 웹 서버처럼 동작하며, 
사용자가 정의한 설정 파일(*.config)에 따라 다양한 환경에서 실행될 수 있습니다. 실행시 config 파일을 따로 argument로 넘기지 않는경우, 이 프로젝트 내부 default.config 파일 기반으로 실행됩니다.
HTTP/1.1 을 기준으로 만들었으며, RFC 7230~7235 까지의 메서드 및 기타 표준 사항등을 엄격히 준수합니다. 단 메서드는 일부 만 구현했습니다.

## ⏲️개발 기간
2023.04.24 ~ 2023.06.15 (2개월)

## ⚙️기능 및 특징
- HTTP/1.1 프로토콜 지원(GET/POST/PUT/DELETE/HEAD 메서드만 구현)
- 정적 파일 서빙
- CGI 처리 지원(.py, perl 등 사용자 지정 언어로 쓰인 파일 필요)
- I/O 멀티플렉싱을 통한 동시 요청 처리
- 사용자 정의 설정 파일을 통한 유연한 구성

## 🦿작동방법
1. repository를 git clone 합니다.
```
git clone https://github.com/Elineely/CreateHTTPServerFromBottom.git
```
2. clone 한 폴더에 들어갑니다.
```
cd CreateHTTPServerFromBottom
```
3. makefile을 실행시켜 컴파일 합니다.
```
make
```
4. 생성된 실행 파일을 실행합니다. 인자가 없는 경우 default.config 파일을 기반으로 웹 서버가 실행됩니다.
```
./cuteWebserv //default.config 기반으로 실행
```
```
./cuteWebserv yourConfigFile.config //사용자가 원하는 설정파일 기반 실행
```

### Prerequisites / 선행 조건

아래 사항들이 설치가 되어있어야합니다.

```
예시
```
