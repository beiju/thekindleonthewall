angular.module('StateManager', ['ui.state',  'be.gauge'])
	.controller('StateController', function($scope, $state, $timeout, config) {
		$scope.auto = localStorage.getItem('auto_state_transition') !== "false";
		
		// Automatically transition to the next state
		$scope.$on('$stateChangeSuccess', function(event, toState, toParams, fromState, fromParams){
			queueTransition(toState.data.nextView);
		});
		$scope.$watch('auto', function(auto, oldAuto) {
			// This can be called before the state is initialized. In that case, this event is unnecessary anyway.
			if (angular.isUndefined($state.current.data)) return;
			
			if (auto) {
				queueTransition($state.current.data.nextView, $scope.nextSwitchTimer);
				countdown();
			} else {
				if ($scope.timeoutPromise) $timeout.cancel($scope.timeoutPromise);
			}
			
			localStorage.setItem('auto_state_transition', auto)
		});
		
		var queueTransition = function(to, delay) {
			$scope.timeoutPromise = $timeout(function() {
				// If we're still in auto mode, complete the transition
				if ($scope.auto) $state.transitionTo( to );
			}, delay || config.autoTransitionDelay);
			
			$scope.nextSwitchTime = Date.now() + (delay || config.autoTransitionDelay);
		}
		
		function countdown() {
			$timeout(function() {
				$scope.nextSwitchTimer = $scope.nextSwitchTime - Date.now();
				if ($scope.auto) countdown();
			}, config.animationUpdateInterval);
		}
		countdown();
	})
	.config(['$urlRouterProvider', '$stateProvider', function($urlRouterProvider, $stateProvider){
		$stateProvider
		    .state('personal-info', {
		        url: "/personal-info",
		        views: {
		            "main": { templateUrl: "personal-info.html" }
		        },
		        data: {
		        	nextView: 'general-info'
		        }
		    })
		    .state('general-info', {
		        url: "/general-info",
		        views: {
		            "main": { templateUrl: "general-info.html" }
		        },
		        data: {
		        	nextView: 'personal-info'
		        }
		        
		    });
	    $urlRouterProvider.otherwise('/personal-info');
	}]);