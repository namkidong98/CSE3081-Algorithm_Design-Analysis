## 정렬 함수
#### 1. IS: Insertion Sort(삽입 정렬)
#### 2. MS: Merge Sort(병합 정렬)
#### 3. QS_NAIVE: Quick Sort_Naive(퀵 정렬, 피벗을 제일 좌측으로 설정)
#### 4. QS_P: Quick Sort_Pivot(퀵 정렬, 피벗을 Median of Three로 설정)
#### 5. QS_PIS: Quick Sort_Pivot + Insertion Sort(4번을 기본으로 일정 원소 이하는 삽입 정렬로 처리)
#### 6. QS_PISTRO: Quick Sort_Pivot + Insertion Sort + Tail Recursion Optimization(5번을 기본으로, 퀵 정렬로 분할할 때 작은 덩어리만 퀵 정렬하고 남은 것은 IS 적용)    
</br>

## 그 외
#### Test generator: 정렬에 사용될 데이터를 만들어내는 코드, 2^N으로 데이터의 크기를 정하고 rd, asc, de, fs으로 총 4가지 유형의 데이터를 만들 수 있다
#### test_data: Test generator를 사용하여 4가지 유형에 따라 만든 데이터로 2^10부터 2^20까지 11개씩 총 44개의 데이터로 구성되어 있다
#### 알고리즘 과제용 데이터(excel): 각 정렬 방법과 데이터 유형에 따라 5번씩 측정하여 평균값을 내고 이를 바탕으로 그래프를 그린 엑셀파일
#### Specification: HW2 과제에 관한 명세서
