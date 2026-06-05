# Hollang(TM)

Hollang(TM)은 Holland(TM)의 프로그래밍 언어입니다.

Hollang(TM)은 Hollang과 실행기인 HLND VM로 구성됩니다.
사용자가 입력한 Hollang 코드를 파이썬 컴파일러로 컴파일하여 HLND VM에서 실행될 수 있는 어썸블리 코드로 변환됩니다

## Hollang 문법
1. 기본 문법
모든 명령어는 함수 - 라인 - 스트림의 구조로 존재한다.
스트림은 하나의 실행 단위라고 보면 되는데, 한 스트림에는 한 명령어만 들어갈 수 있다.
스트림 안의 성분은 '\\\\' 을 통해 구분한다. 
스트림을 열 때는 //open, 닫을 때는 close//을 쓴다.
예컨대, Hello, World!를 출력하는 스트림은 이렇게 쓴다
> //open \\ write \\ ’Hello, World!’ \\ close//

라인은 콘솔에서 출력되는 한 줄을 말한다. 즉, 여러 스트림이 한 줄을 출력할 수 있다.
다음 라인으로 넘어가기 위해서는 //drop//를 입력한다.
예를 들어, Hello,를 입력한 뒤 개행하여 World!를 입력하는 명령어는 다음과 같다.
//open \\ write \\ ‘Hello,’ \\ close//
//drop//
//open \\ write \\ ‘World!’ \\ close//
라고 입력한다.

함수는 여러 라인과 스트림을 하나로 묶어놓은 말 그대로 함수이다.
함수는 숫자를 반환하는 num 함수, 문자를 반환하는 cha 함수, 그리고 코드를 실행하는 main 함수가 있다.
함수를 선언할 때는 //def \\ 함수 유형 \\ 함수명 \\  매개변수~~ \\ continue// 를 사용하며, //def \\ close//로 함수 선언을 끝낸다.
즉, 우리가 Hello, World!를 실행하기 위해 필요한 코드는 다음과 같다.
//def \\ main \\ main \\ continue//
//open \\ write \\ ‘Hello, World’ \\ close//
//def \\ close//
또한, 우리가 숫자를 문자열로 변환하는 함수는 이렇게 쓸 수 있겠다.
//def \\ cha \\ toString \\ num**k1 \\ continue//
//alloc \\ cha**result \\ k1 \\ save//
//def \\ result \\ close//


2. 표준입출력
가장 기본적인 명령어는 write이다.
write는 스트림을 단위로 한다.
안의 매개변수를 출력하는 명령어다. 단, 매개변수 하나당 한 단위만 들어갈 수 있다.
한 단위라는 게 뭐냐면, 문자열, 변수, 숫자들 중 하나의 유형만 매개변수 하나에 들어갈 수 있다는 것이다.
해당 명령어로 딸기 13개를 출력하는 코드는 다음과 같다.
//open \\ ‘딸기’ \\ 13 \\ ‘개’ \\ close//

입력하는 명령어는 read이다.
read도 스트림을 단위로 한다.
매개변수에 할당된 변수에 들어가는 내용을 입력받는데, 여러 매개변수가 들어간 경우 기본적으로 띄어쓰기를 기준으로 구분되게되나, \\ sep=’(내용)’ \\ 을 이용해 구분자를 바꿀 수 있다.
즉, 숫자 세 개를 입력받아 각각 a,b,c의 값으로 설정하는 코드는 다음과 같다.
//alloc \\ num**a \\ save//
//alloc \\ num**b \\ save//
//alloc \\ num**c \\ save//
//read \\ a \\ b \\ c \\ close//
여기서 쉼표를 구분자로 입력받고 싶다면 다음처럼 하면 된다.
//alloc \\ num**a \\ save//
//alloc \\ num**b \\ save//
//alloc \\ num**c \\ save//
//read \\ a \\ b \\ c \\ sep=‘,’ \\ close//

3. 자료형
언어에서 변수는 두 가지 유형이 있다
숫자형 (num) 이랑 문자형 (cha)
변수 관리 명령어는 alloc인데, alloc은 스트림이나 라인 없이 함수 아래에 독립적으로 작동한다.
//alloc \\ 자료형**변수이름 \\ 매개변수 \\ 명령어// 형식으로 입력한다.
명령어는 변수를 선언하거나 매개변수의 값을 지정하는 save//
두 개의 숫자형 변수의 값을 더해 특정한 위치에 저장하는 add//
두 개의 문자형 변수를 서로 하나의 문자열로 합쳐 특정한 위치에 저장하는 join//
그리고 변수를 삭제하여 메모리를 정리하는 wipe//가 있다.

4. 로직
판단문은 크게 if와 else 두 가지가 존재한다. 생겨먹은것 만으로 어떻게 동작하는지는 대략 알 것 같기 때문에 설명은 생략하도록 하겠다.
로직은 함수로 취급하기 때문에 //def로 선언해야된다.
//def \\ logic \\ if \\ (판단문) \\ continue// ~~ //def \\ close// 정도의 형태인 것이다.
예를 들어, 비밀번호를 입력받아서 비밀번호가 맞다면 인사를 하는 프로그램은 다음과 같다.
//write \\ ‘Enter password’ \\ close//
//alloc \\ num**pw \\ save//
//def \\ logic \\ if \\ pw==1234 \\ continue//
//write \\ ‘Hello, admin!’ \\ close//
//def \\ close//
else는 저렇게 거창하게 쓸 필요 없이 //def \\ else \\ continue// 를 쓰면 된다.
예를 들어, 비밀번호가 틀렸다면 틀렸다고 알려주는 프로그램은 다음과 같다.
//write \\ ‘Enter password’ \\ close//
//alloc \\ num**pw \\ save//
//def \\ logic \\ if \\ pw==1234 \\ continue//
//write \\ ‘Hello, admin!’ \\ close//
//def \\ else \\ continue//
//write \\ ‘꺼져’ \\ close//
//def \\ close//
//def \\ close//

반복문은 while이 있다. 판단문이 거짓이 될 때까지 명령어를 반복하여 실행하는 함수이다.
예를 들면, 딸기를 5번 출력하는 명령어는 다음이 될 것이다.
//def \\ logic \\ while \\ num**i==0 \\ i<5 \\ continue//
//write \\ ‘딸기’ \\ close//
//alloc \\ num**i \\ i+1 \\ close//
//def \\ close//

5. 컴파일러
모든 코드는 HLND(TM) VM에서 실행될 수 있는 명령어로 컴파일된다
이 모든 것을 종합해서 아까 만든 비밀번호 프로그램의 전체 코드는 다음과 같게 된다.

//def \\ main \\ main \\ continue//
	//write \\ ‘Enter password’ \\ close//
	//alloc \\ num**pw \\ save//
	//def \\ logic \\ if \\ pw==1234 \\ continue//
		//write \\ ‘Hello, admin!’ \\ close//
		//def \\ else \\ continue//
			//write \\ ‘꺼져’ \\ close//
		//def \\ close//
	//def \\ close//
//def \\ close//

들여쓰기는 딱히 할 필요는 없는데 가독성을 위해 들여쓰기를 허용한다.

컴파일러를 실행하면 작업 디렉터리의 code 폴더에서 main.holland 파일을 불러온다.
컴파일이 끝난 파일은 main.hlnd로 저장되며, 작업 디렉터리의 runtime 폴더에 저장된다.

## 코드 작성 방법
제공된 파일 속 code 폴더 안에 있는 main.holland에 코드를 작성하시면 됩니다.

## 실행 방법
hlnd_launch.cpp를 알아서 잘 빌드하신 다음에 아래 실행 명령어를 터미널이나 CMD에 입력하시면 컴파일과 실행이 자동으로 됩니다.

## macOS & Linux 컴파일 명령어
> python3 hlnd_compile.py
## 실행 명령어
> ./hlnd_launch
## Windows 컴파일 명령어
> python3 hlnd_compile.py
## 실행 명령어
> ./hlnd_launch.exe
